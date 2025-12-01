#include "SchemaManager.h"
#include "DbUtils.h"
#include <string>
#include <sqlite3.h>

bool SchemaManager::initialize(DbResult& result) {
    // Always enforce foreign keys
    if (!db_.exec("PRAGMA foreign_keys = ON;", result))
        return false;

    // Ensure SchemaVersion table exists
    if (!db_.exec(R"SQL(
        CREATE TABLE IF NOT EXISTS SchemaVersion (
            Version INTEGER PRIMARY KEY,
            AppliedOn TEXT NOT NULL,
            Description TEXT
        );
    )SQL", result)) return false;

    // Check current version
    int version = 0;
    sqlite3_stmt* stmt = nullptr;
    if (db_.prepare("SELECT MAX(Version) FROM SchemaVersion;", stmt, result)) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            version = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    // Apply baseline schema if fresh
    if (version < 1) {
        const char* baseline = R"SQL(
            CREATE TABLE IF NOT EXISTS Categories (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE,
                Description TEXT
            );

            CREATE TABLE IF NOT EXISTS Manufacturers (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE,
                Country TEXT,
                Website TEXT,
                Notes TEXT
            );

            CREATE TABLE IF NOT EXISTS Components (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                PartNumber TEXT NOT NULL,
                Description TEXT,
                CategoryID INTEGER NOT NULL,
                ManufacturerID INTEGER,
                Quantity INTEGER DEFAULT 0,
                Notes TEXT,
                CreatedOn TEXT NOT NULL DEFAULT (datetime('now')),
                ModifiedOn TEXT NOT NULL DEFAULT (datetime('now')),
                FOREIGN KEY (CategoryID) REFERENCES Categories(ID),
                FOREIGN KEY (ManufacturerID) REFERENCES Manufacturers(ID)
            );

            CREATE TABLE IF NOT EXISTS ResistorComposition (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE
            );

            CREATE TABLE IF NOT EXISTS ResistorPackage (
                ID INTEGER PRIMARY KEY AUTOINCREMENT,
                Name TEXT NOT NULL UNIQUE
            );

            CREATE TABLE IF NOT EXISTS Resistors (
                ComponentID INTEGER PRIMARY KEY,
                Resistance REAL NOT NULL,
                Tolerance REAL,
                PowerRating REAL,
                TempCoefficient REAL,
                PackageTypeID INTEGER,
                CompositionID INTEGER,
                LeadSpacing REAL,
                VoltageRating REAL,
                FOREIGN KEY (ComponentID) REFERENCES Components(ID) ON DELETE CASCADE,
                FOREIGN KEY (PackageTypeID) REFERENCES ResistorPackage(ID),
                FOREIGN KEY (CompositionID) REFERENCES ResistorComposition(ID)
            );

            CREATE TRIGGER IF NOT EXISTS update_component_modified
            AFTER UPDATE ON Components
            FOR EACH ROW
            BEGIN
                UPDATE Components
                SET ModifiedOn = datetime('now')
                WHERE ID = OLD.ID;
            END;
        )SQL";

        if (!db_.exec(baseline, result)) return false;

        // Insert baseline version record
        sqlite3_stmt* insertStmt = nullptr;
        if (db_.prepare(
            "INSERT INTO SchemaVersion (Version, AppliedOn, Description) VALUES (?,?,?);",
            insertStmt,
            result)) {

            sqlite3_bind_int(insertStmt, 1, 1);
            sqlite3_bind_text(insertStmt, 2, currentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3,
                "Baseline schema with Categories, Manufacturers, Components.Quantity, CreatedOn/ModifiedOn, and trigger.",
                -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
            }

            sqlite3_finalize(insertStmt);
        }
    }

    // Future migrations go here:
    if (version < 2) {
        const char* migration2 = R"SQL(
        ALTER TABLE Components
        ADD COLUMN DatasheetLink TEXT;
    )SQL";

        if (!db_.exec(migration2, result)) return false;

        sqlite3_stmt* insertStmt = nullptr;
        if (db_.prepare(
            "INSERT INTO SchemaVersion (Version, AppliedOn, Description) VALUES (?,?,?);",
            insertStmt,
            result)) {

            sqlite3_bind_int(insertStmt, 1, 2);
            sqlite3_bind_text(insertStmt, 2, currentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3,
                "Added DatasheetLink column to Components table.",
                -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
            }
            sqlite3_finalize(insertStmt);
        }
    }

    if (version < 3) {
        const char* migration3 = R"SQL(
        -- Lookup tables
        CREATE TABLE IF NOT EXISTS CapacitorDielectric (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS CapacitorPackage (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE
        );

        -- General capacitor attributes
        CREATE TABLE IF NOT EXISTS Capacitors (
            ComponentID INTEGER PRIMARY KEY,
            Capacitance REAL NOT NULL,
            VoltageRating REAL,
            Tolerance REAL,
            ESR REAL,
            LeakageCurrent REAL,
            Polarized INTEGER, -- 0 = No, 1 = Yes
            PackageTypeID INTEGER,
            DielectricTypeID INTEGER,
            FOREIGN KEY (ComponentID) REFERENCES Components(ID) ON DELETE CASCADE,
            FOREIGN KEY (PackageTypeID) REFERENCES CapacitorPackage(ID),
            FOREIGN KEY (DielectricTypeID) REFERENCES CapacitorDielectric(ID)
        );

        -- Electrolytic subtype with geometry
        CREATE TABLE IF NOT EXISTS Electrolytics (
            ComponentID INTEGER PRIMARY KEY,
            Diameter REAL,
            Height REAL,
            LeadSpacing REAL,
            FOREIGN KEY (ComponentID) REFERENCES Capacitors(ComponentID) ON DELETE CASCADE
        );
    )SQL";

        if (!db_.exec(migration3, result)) return false;

        sqlite3_stmt* insertStmt = nullptr;
        if (db_.prepare(
            "INSERT INTO SchemaVersion (Version, AppliedOn, Description) VALUES (?,?,?);",
            insertStmt,
            result)) {

            sqlite3_bind_int(insertStmt, 1, 3);
            sqlite3_bind_text(insertStmt, 2, currentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3,
                "Added Capacitors table with dielectric/package lookups and Electrolytics subtype.",
                -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
            }
            sqlite3_finalize(insertStmt);
        }
    }

    if (version < 4) {
        const char* migration4 = R"SQL(
        -- Lookup tables for transistors
        CREATE TABLE IF NOT EXISTS TransistorType (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS TransistorPolarity (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS TransistorPackage (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            Name TEXT NOT NULL UNIQUE
        );

        -- Base transistor table
        CREATE TABLE IF NOT EXISTS Transistors (
            ComponentID INTEGER PRIMARY KEY,
            TypeID INTEGER NOT NULL,
            PolarityID INTEGER NOT NULL,
            PackageID INTEGER,
            FOREIGN KEY (ComponentID) REFERENCES Components(ID) ON DELETE CASCADE,
            FOREIGN KEY (TypeID) REFERENCES TransistorType(ID),
            FOREIGN KEY (PolarityID) REFERENCES TransistorPolarity(ID),
            FOREIGN KEY (PackageID) REFERENCES TransistorPackage(ID)
        );

        -- BJT subtype table
        CREATE TABLE IF NOT EXISTS BJTs (
            ComponentID INTEGER PRIMARY KEY,
            VceMax REAL,
            IcMax REAL,
            PdMax REAL,
            Hfe REAL,
            Ft REAL,
            FOREIGN KEY (ComponentID) REFERENCES Transistors(ComponentID) ON DELETE CASCADE
        );

        -- Future expansion: MOSFETs, JFETs, IGBTs
        -- Each will follow the same pattern: FK to Transistors.ComponentID
        -- with only the fields relevant to that subtype.
    )SQL";

        if (!db_.exec(migration4, result)) return false;

        sqlite3_stmt* insertStmt = nullptr;
        if (db_.prepare(
            "INSERT INTO SchemaVersion (Version, AppliedOn, Description) VALUES (?,?,?);",
            insertStmt,
            result)) {

            sqlite3_bind_int(insertStmt, 1, 4);
            sqlite3_bind_text(insertStmt, 2, currentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3,
                "Added Transistors base table with lookup tables and BJT subtype. Future expansion for MOSFETs, JFETs, IGBTs.",
                -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
            }
            sqlite3_finalize(insertStmt);
        }
    }

    if (version < 5) {
        const char* migration5 = R"SQL(
        -- Seed Categories
        INSERT OR IGNORE INTO Categories (Name, Description) VALUES
            ('Resistor','Resistive components'),
            ('Capacitor','Capacitive components'),
            ('Transistor','Active transistor devices'),
            ('Diode','Semiconductor diodes'),
            ('Fuse','Circuit protection devices');

        -- Seed Manufacturers
        INSERT OR IGNORE INTO Manufacturers (Name, Country, Website, Notes) VALUES
            ('Generic','USA','','Baseline manufacturer'),
            ('Vishay','USA','https://www.vishay.com','Major resistor/capacitor vendor'),
            ('Murata','Japan','https://www.murata.com','Capacitor vendor'),
            ('ON Semiconductor','USA','https://www.onsemi.com','Transistor vendor'),
            ('Sanken','Japan','https://www.sanken-ele.co.jp/en/','Power semiconductors, ICs, LEDs, power supplies'),
            ('Toshiba','Japan','https://www.global.toshiba/ww/outline/corporate.html','Broad electronics conglomerate, semiconductors, storage, power systems'),
            ('Stackpole','USA','https://www.seielect.com','Resistive components manufacturer, thick/thin film, current sense resistors'),
            ('TE Connectivity','Ireland/USA','https://www.te.com','Connectivity and sensor solutions for automotive, aerospace, industrial'),
            ('Panasonic','Japan','https://www.panasonic.com/global/about/corporate-profile/overview.html','Global electronics giant, consumer and industrial devices'),
            ('TDK Electronics','Germany','https://www.tdk-electronics.tdk.com/en','Electronic components: capacitors, inductors, sensors, EMC filters'),
            ('Rubycon','Japan','https://www.rubycon.co.jp/en/','Capacitors and power supplies, aluminum electrolytic and film types'),
            ('KEMET','USA','https://www.kemet.com','Capacitors (tantalum, ceramic, aluminum, film), subsidiary of Yageo'),
            ('SCHURTER','Switzerland/USA','https://www.schurter.com/en','Circuit protection, connectors, switches, EMC products'),
            ('Bel Fuse','USA','https://www.belfuse.com/company/about-us','Power, protection, and connectivity solutions for telecom, aerospace, industrial');

        -- Seed Resistor lookups
        INSERT OR IGNORE INTO ResistorComposition (Name) VALUES
            ('Carbon Film'),('Metal Film'),('Wirewound');

        INSERT OR IGNORE INTO ResistorPackage (Name) VALUES
            ('Axial leaded'),
            ('0603'),
            ('0805');

        -- Seed Capacitor lookups
        INSERT OR IGNORE INTO CapacitorDielectric (Name) VALUES
            ('C0G/NP0'),('X7R'),('Y5V'),
            ('Polypropylene'),('Polyester'),
            ('Aluminum'),('Tantalum'),('Mica');

        INSERT OR IGNORE INTO CapacitorPackage (Name) VALUES
            ('Radial leaded'),('Axial leaded'),('SMD');

        -- Seed Transistor lookups
        INSERT OR IGNORE INTO TransistorType (Name) VALUES
            ('BJT'),('MOSFET'),('JFET'),('IGBT');

        INSERT OR IGNORE INTO TransistorPolarity (Name) VALUES
            ('NPN'),('PNP'),('N-Channel'),('P-Channel');

        INSERT OR IGNORE INTO TransistorPackage (Name) VALUES
            ('TO-3P'),('TO-220'),('TO-92'),('TO-126');
    )SQL";

        if (!db_.exec(migration5, result)) return false;

        sqlite3_stmt* insertStmt = nullptr;
        if (db_.prepare(
            "INSERT INTO SchemaVersion (Version, AppliedOn, Description) VALUES (?,?,?);",
            insertStmt,
            result)) {

            sqlite3_bind_int(insertStmt, 1, 5);
            sqlite3_bind_text(insertStmt, 2, currentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3,
                "Seeded baseline lookup values for categories, manufacturers, resistor, capacitor, and transistor tables.",
                -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
            }
            sqlite3_finalize(insertStmt);
        }
    }

    if (version < 6) {
        const char* migration6 = R"SQL(
        -- Create Fuse lookup tables
        CREATE TABLE IF NOT EXISTS FusePackage (
            Id INTEGER PRIMARY KEY,
            Name TEXT UNIQUE NOT NULL
        );

        CREATE TABLE IF NOT EXISTS FuseType (
            Id INTEGER PRIMARY KEY,
            Name TEXT UNIQUE NOT NULL
        );

        -- Create Fuses table linked to Components
        CREATE TABLE IF NOT EXISTS Fuses (
            ComponentId INTEGER PRIMARY KEY,
            PackageId INTEGER,
            TypeId INTEGER,
            FOREIGN KEY(ComponentId) REFERENCES Components(Id) ON DELETE CASCADE,
            FOREIGN KEY(PackageId) REFERENCES FusePackage(Id),
            FOREIGN KEY(TypeId) REFERENCES FuseType(Id)
        );

        -- Seed Fuse types
        INSERT OR IGNORE INTO FuseType (Name) VALUES
            ('Fast-blow'),
            ('Slow-blow'),
            ('Resettable (polyfuse)');

        -- Seed Fuse packages
        INSERT OR IGNORE INTO FusePackage (Name) VALUES
            ('Axial'),
            ('Radial'),
            ('Cartridge'),
            ('SMD');
    )SQL";

        if (!db_.exec(migration6, result)) return false;

        sqlite3_stmt* insertStmt = nullptr;
        if (db_.prepare(
            "INSERT INTO SchemaVersion (Version, AppliedOn, Description) VALUES (?,?,?);",
            insertStmt,
            result)) {

            sqlite3_bind_int(insertStmt, 1, 6);
            sqlite3_bind_text(insertStmt, 2, currentTimestamp().c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(insertStmt, 3,
                "Added Fuse support: created FusePackage, FuseType, and Fuses tables with baseline seeds.",
                -1, SQLITE_TRANSIENT);

            if (sqlite3_step(insertStmt) != SQLITE_DONE) {
                result.setError(sqlite3_errcode(db_.handle()), sqlite3_errmsg(db_.handle()));
            }
            sqlite3_finalize(insertStmt);
        }
    }

    return true;
}
