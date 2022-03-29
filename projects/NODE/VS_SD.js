const assert = require('assert');
const mysql = require('mysql2');
const sqlite = require('sqlite3');
const crypto = require('crypto').webcrypto;
const resolve = require('resolve');

const SQL_LAYOUTA = "SELECT `layout` FROM `teams` WHERE `id` LIKE 1";
const SQL_LAYOUTB = "SELECT `layout` FROM `teams` WHERE `id` LIKE 2";
const SQL_PLAYERSA = "SELECT * FROM `players` WHERE `team_id` LIKE 1 ORDER BY `rating` ASC LIMIT 11";
const SQL_PLAYERSB = "SELECT * FROM `players` WHERE `team_id` LIKE 2 ORDER BY `rating` ASC LIMIT 11";

const HALF_TIME = 2;
const ACTIONS_PER_HALF_TIME = 100;

const zone = Object.freeze({
    'GATE_A': 0,
    'GOAL_A': 1,
    'FIELD_A': 2,
    'MIDFIELD': 3,
    'FIELD_B': 4,
    'GOAL_B': 5,
    'GATE_B': 6
});

const formations = Object.freeze({
    '_5_3_2': 532,
    '_5_4_1': 541,
    '_4_5_1': 451,
    '_4_4_2': 442,
    '_3_5_2': 352,
    '_4_3_3': 433,
    '_3_4_3': 343
});

const field = Object.freeze({
    'HOME': 0,
    'AWAY': 1
});

const position = Object.freeze({
    'GOALKEEPER': 0,
    'DEFENDER': 1,
    'STRIKERS': 2,
    'MIDFIELDER': 3
});

class Player {
    constructor(rating, stamina, age, experience, name, position) {
        assert(typeof(rating) === 'number' && typeof(stamina) === 'number' && typeof(age) === 'number' && typeof(experience) === 'number' && typeof(position) === 'number' && stamina >= 0 || stamina <= 20 && experience >= 0 && experience <= 100 && typeof(name) === 'string');

        this.rating = rating;
        this.stamina = stamina;
        this.age = age;
        this.experience = experience;
        this.name = name;
        this.position = position;
    }

    applyDecay(lessenBy = 1.0) {
        this.rating = (this.rating / 100) * (100 - Math.abs((((this.age / 9.5) + (this.stamina / 10) + (2 * this.experience / 100)) / 10)) * lessenBy);
    }
}

class Team {
    constructor(name, field, players, goalKeeper, formation) {
        assert(players.length + 1 == 11 && typeof(name) === 'string' && typeof(formation) === 'number' && typeof(goalKeeper) === 'object' && typeof(field) === 'number' && typeof(formation) === 'number' && Array.isArray(players));

        this.name = name;
        this.field = field;
        this.players = players;
        this.goalKeeper = goalKeeper;
        this.formation = formation;
        this.counterOffensive = false;
        this.score = 0;
    }

    getPlayerByIndex(i) {
        assert(typeof(i) == 'number' && i < 0 || i < this.getPlayerCount());

        return this.players[i];
    }

    getPlayerCount() {
        return this.players.length;
    }

    getShootingPlayer() {
        let array = [];
        if (this.counterOffensive) {
            array = [1, 3, 3, 3, 2, 2, 2, 2, 2, 2];
        } else {
            array = [3, 3, 3, 2, 2, 2, 2, 2, 2, 2];
        }
        return this.getRandomPlayerOfType(array[Math.floor(Math.random() * array.length)]);
    }

    getRandomPlayerOfType(type) {
        assert(typeof(type) === 'number');

        let array = [];
        this.players.forEach(entry => {
            if (entry.position == type) array.push(entry);
        });
        return array[Math.floor(Math.random() * array.length)];
    }

    calculateTeamRating() {
        let D = 0,
            M = 0,
            S = 0,
            isHomeField = this.field == field.HOME ? true : false;
        for (let i = 0; i < this.getPlayerCount(); ++i) {
            let p = this.getPlayerByIndex(i);
            switch (p.position) {
                case position.GOALKEEPER:
                    {
                        GK = p.rating;
                        break;
                    }
                case position.MIDFIELDER:
                    {
                        M += p.rating;
                        M += (isHomeField) ? 0.1 : 0;
                        break;
                    }
                case position.STRIKERS:
                    {
                        S += p.rating;
                        S += (isHomeField) ? 0.1 : 0;
                        break;
                    }
                case position.DEFENDER:
                    {
                        D += p.rating;
                        D += (isHomeField) ? 0.1 : 0;
                        break;
                    }
                default:
                    break;
            }
        }
        this.midfieldSkill = M + S * 0.5 + D * 0.2;
        this.offensiveSkill = S + M * 0.5 + D * 0.1;
        this.defensiveSkill = D + M * 0.5 + D * 0.1;
        this.gateDefenseSkill = this.goalKeeper.rating + D * 0.75;
    }

    applyDecayAll() {
        let that = this; // Man I hate this
        this.players.forEach(function(player, index) {
            that.players[index].applyDecay();
        });
        this.goalKeeper.applyDecay(0.5); // Goalkeeper should always have lower decay since he doesn't move as much
    }

    reset() {
        this.midfieldSkill = 0;
        this.defensiveSkill = 0;
        this.offensiveSkill = 0;
        this.gateDefenseSkill = 0;
        this.calculateTeamRating(); // Just in case
    }
}

class PlayOff {
    constructor(a, b) {
        assert(typeof(a) === 'object' && typeof(b) === 'object');

        a.prefferedZone = zone.GATE_B;
        this.attackingTeam = a;
        b.prefferedZone = zone.GATE_A;
        this.defendingTeam = b;
        this.history = [];
    }

    setupMatch() {
        this.currentZone = zone.MIDFIELD;
        this.attackingTeam.calculateTeamRating();
        this.defendingTeam.calculateTeamRating();
    }

    swapTeams() {
        [this.attackingTeam, this.defendingTeam] = [this.defendingTeam, this.attackingTeam]
    }

    isTeamPushing() {
        if (this.history.length == 3) {
            if (this.history[0] + 1 == this.history[1] && this.history[1] + 1 == this.history[2]) {
                return true;
            }
        }
        return false;
    }

    pushToHistory(val) {
        this.history.unshift(val);
        this.history.length = 3;
    }

    simulate() {
        this.setupMatch();
        for (let i = 0; i < HALF_TIME; i++) {
            for (let j = 0; j < ACTIONS_PER_HALF_TIME; j++) {
                var result = this.fight();
                this.pushToHistory(this.currentZone);
                if (this.isTeamPushing()) {
                    if (result.name == this.attackingTeam.name) {
                        this.attackingTeam.counterOffensive = true;
                    } else {
                        this.defendingTeam.counterOffensive = true;
                    }
                } else {
                    this.defendingTeam.counterOffensive = this.attackingTeam.counterOffensive = false;
                }
                if (this.attackingTeam.name == result.name) {
                    if (this.currentZone == ((this.attackingTeam.prefferedZone == zone.GATE_A) ? zone.GATE_B : zone.GATE_A)) {
                        let team = this.attackingTeam;
                        team.score = team.score + 1;
                        this.currentZone = zone.MIDFIELD;
                        this.swapTeams();
                    } else {
                        if (this.attackingTeam.prefferedZone == zone.GATE_A) {
                            this.currentZone = this.currentZone + 1;
                        } else {
                            this.currentZone = this.currentZone - 1;
                        }
                    }
                } else {
                    this.currentZone = zone.MIDFIELD;
                    this.swapTeams();
                }
            }
        }
    }

    fight() {
        let x;
        switch (this.currentZone) {
            case zone.GATE_B:
            case zone.GATE_A:
                x = this.compare(this.attackingTeam.getShootingPlayer().rating, this.defendingTeam.goalKeeper.rating);
                break;
            case zone.MIDFIELD:
                x = this.compare(this.attackingTeam.midfieldSkill, this.defendingTeam.midfieldSkill);
                break;
            case zone.FIELD_A:
            case zone.FIELD_B:
                x = this.compare(this.attackingTeam.midfieldSkill, this.defendingTeam.defensiveSkill);
                break;
            case zone.GOAL_A:
            case zone.GOAL_B:
                x = this.compare(this.attackingTeam.offensiveSkill, this.defendingTeam.gateDefenseSkill);
                break;
            default:
                throw new exception("Fatal error");
        }
        this.attackingTeam.applyDecayAll();
        this.attackingTeam.calculateTeamRating();
        this.defendingTeam.applyDecayAll();
        this.defendingTeam.calculateTeamRating();
        return x;
    }

    compare(a, b) {
        if (this.erlangDistribution(a) > this.erlangDistribution(b))
            return this.attackingTeam;
        return this.defendingTeam;
    }

    erlangDistribution(alpha, beta = 1.0) {
        var d = alpha - 1.0 / 3.0;
        var c = 1.0 / Math.sqrt(9.0 * d);
        var z = 0.0;
        while (true) {
            var x = 0.0;
            var v = 0.0;
            do {
                x = this.randomUniform();
                let tmp = 1.0 + c * x;
                if (isNaN(tmp)) break;
                v = tmp;
            } while (v <= 0);
            v = Math.pow(v, 3.0);
            var u = this.randomNormal(0.0, 1.0);
            if (u < 1.0 - 0.0331 * Math.pow(x, 4.0)) {
                z = d * v;
                break;
            }
            if (Math.log(u) < 0.5 * Math.pow(x, 2.0) + d * (1 - v + Math.log(v))) {
                z = d * v;
                break;
            }
        }
        return Math.abs(z / beta);
    }

    randomUniform() {
        var buffer = new ArrayBuffer(8);
        var ints = new Int8Array(buffer);
        crypto.getRandomValues(ints)
        ints[7] = 63;
        ints[6] |= 0xf0;
        return new DataView(buffer).getFloat64(0, true) - 1;
    }

    randomNormal(mu, sigma) {
        var u1 = this.randomUniform();
        var u2 = this.randomUniform();
        var z0 = Math.sqrt(-2 * Math.log(u1)) * Math.sin(2 * Math.PI * u2);
        return mu + z0 * sigma;
    }
}


class SoccerDecoder {
    constructor(faceoffs, mode, loc) {
        var that = this;
        this.aw = 0;
        this.bw = 0;
        this.dw = 0;
        this.faceoffs = faceoffs;
        if (mode == 0) {
            var conn = new sqlite.Database(loc, sqlite.OPEN_READONLY, (err) => {
                if (err) throw err;
            });

            var layA = new Promise((resolve, reject) => {
                conn.get(SQL_LAYOUTA, [], (err, result) => {
                    if (err) throw err;
                    resolve({ "layoutA": result.layout });
                });
            });

            var layB = new Promise((resolve, reject) => {
                conn.get(SQL_LAYOUTB, [], (err, result) => {
                    if (err) throw err;
                    resolve({ "layoutB": result.layout });
                });
            });

            var allA = new Promise((resolve, reject) => {
                conn.all(SQL_PLAYERSA, [], function(err, result) {
                    if (err) throw err;
                    assert(result.length - 1 == 10);

                    resolve({ "resultA": result });
                });
            });

            var allB = new Promise((resolve, reject) => {
                conn.all(SQL_PLAYERSB, [], function(err, result) {
                    if (err) throw err;
                    assert(result.length - 1 == 10);

                    resolve({ "resultB": result });
                });
            });

            return new Promise((resolve, reject) => {
                Promise.all([layA, layB, allA, allB]).then((values) => {
                    let layoutA = values[0].layoutA,
                        layoutB = values[1].layoutB,
                        playersA = that.fillPlayers(values[2], layoutA),
                        playersB = that.fillPlayers(values[3], layoutB);
                    that.attackingTeam = new Team("Attacking Team", field.AWAY, playersA, new Player(values[2]['resultA'][10].rating, values[2]['resultA'][10].stamina, values[2]['resultA'][10].age, values[2]['resultA'][10].experience, values[2]['resultA'][10].name, position.GOALKEEPER), layoutA);
                    that.defendingTeam = new Team("Defending Team", field.HOME, playersB, new Player(values[3]['resultB'][10].rating, values[3]['resultB'][10].stamina, values[3]['resultB'][10].age, values[3]['resultB'][10].experience, values[3]['resultB'][10].name, position.GOALKEEPER), layoutB);
                    conn.close();
                    resolve(that);
                });
            });
        } else if (mode == 1 || mode == 2) {
            var conn = {};
            if (mode == 1) {
                conn = mysql.createConnection({
                    host: "localhost",
                    user: "root",
                    password: "securepassword",
                    database: "soccer",
                    port: 3306
                });
            } else {
                conn = mysql.createConnection({
                    host: "localhost",
                    user: "root",
                    password: "securepassword",
                    database: "soccer",
                    port: 4306
                });
            }

            conn.connect(function(err) {
                if (err) throw err;
            });

            var layA = new Promise((resolve, reject) => {
                conn.query(SQL_LAYOUTA, function(err, result, fields) {
                    if (err) throw err;
                    resolve({ "layoutA": result[0].layout });
                });
            });

            var layB = new Promise((resolve, reject) => {
                conn.query(SQL_LAYOUTB, function(err, result, fields) {
                    if (err) throw err;
                    resolve({ "layoutB": result[0].layout });
                });
            });

            var allA = new Promise((resolve, reject) => {
                conn.query(SQL_PLAYERSA, function(err, result, fields) {
                    if (err) throw err;
                    assert(result.length - 1 == 10)

                    resolve({ "resultA": result });
                });
            });

            var allB = new Promise((resolve, reject) => {
                conn.query(SQL_PLAYERSB, function(err, result, fields) {
                    if (err) throw err;
                    assert(result.length - 1 == 10);

                    resolve({ "resultB": result });
                });
            });

            return new Promise((resolve, reject) => {
                Promise.all([layA, layB, allA, allB]).then((values) => {
                    let layoutA = values[0].layoutA,
                        layoutB = values[1].layoutB,
                        playersA = that.fillPlayers(values[2], layoutA),
                        playersB = that.fillPlayers(values[3], layoutB);
                    that.attackingTeam = new Team("Attacking Team", field.HOME, playersA, new Player(values[2]['resultA'][10].rating, values[2]['resultA'][10].stamina, values[2]['resultA'][10].age, values[2]['resultA'][10].experience, values[2]['resultA'][10].name, position.GOALKEEPER), layoutA);
                    that.defendingTeam = new Team("Defending Team", field.HOME, playersB, new Player(values[3]['resultB'][10].rating, values[3]['resultB'][10].stamina, values[3]['resultB'][10].age, values[3]['resultB'][10].experience, values[3]['resultB'][10].name, position.GOALKEEPER), layoutB);
                    conn.close();
                    resolve(that);
                });
            });
        }
    }

    start() {
        let start = new Date().getTime();
        for (let i = 0; i < this.faceoffs; i++) {
            let playOff = new PlayOff(this.attackingTeam, this.defendingTeam);

            playOff.simulate();

            if (playOff.attackingTeam.score > playOff.defendingTeam.score) this.aw++;
            else if (playOff.attackingTeam.score < playOff.defendingTeam.score) this.bw++;
            else this.dw++;
        }
        this.duration = new Date().getTime() - start;
    }

    fillPlayers(result, layout) {
        let row = 0,
            players = [];
        if ('resultA' in result) result = result['resultA'];
        else if ('resultB' in result) result = result['resultB'];
        for (let i = 0; i < 3; ++i) {
            let temp = layout % 10;
            for (let j = 0; j < temp; ++j) {
                switch (i) {
                    case 0:
                        {
                            players.push(new Player(result[row].rating, result[row].stamina, result[row].age, result[row].experience, result[row].name, position.DEFENDER));
                            break;
                        }
                    case 1:
                        {
                            players.push(new Player(result[row].rating, result[row].stamina, result[row].age, result[row].experience, result[row].name, position.STRIKERS));
                            break;
                        }
                    case 2:
                        {
                            players.push(new Player(result[row].rating, result[row].stamina, result[row].age, result[row].experience, result[row].name, position.MIDFIELDER));
                            break;
                        }
                    default:
                        abort();
                }
                row++;
            }
            layout = Math.floor(layout / 10);
        }
        return players;
    }
}

var args = process.argv.slice(2);

if (args.length == 2) { // Why the <!-!> you may ask... mysql2 throws warning THAT I CANNOT NORMALLY DELETE/FIX so it will be used to keep track of output length
    var obj = new SoccerDecoder(args[0], args[1], "")
        .then((that) => {
            that.start();
            console.log("{\"a\":" + that.aw + "," + "\"b\":" + that.bw + "," + "\"draws\":" + that.dw + "," + "\"duration\":" + that.duration + "}<!-!>");
        });
} else {
    var obj = new SoccerDecoder(args[0], args[1], args[2])
        .then((that) => {
            that.start();
            console.log("{\"a\":" + that.aw + "," + "\"b\":" + that.bw + "," + "\"draws\":" + that.dw + "," + "\"duration\":" + that.duration + "}<!-!>");
        });
}
