/**
 * @file
 *
 * @brief Utilities for querying and manipulating game objects.
 *
 * @subsubsection ObjListeners Object Event Listeners
 *
 * The AER framework allows mods to attach an arbitrary number of
 * listeners to the same event on the same object. This is accomplished
 * by wrapping vanilla event listeners in "event traps."
 *
 * Whenever a mod attaches an event listener to an object, it is added to
 * that event trap's "stream"; either "upstream" or "downstream" (which
 * one is controlled by the attachment function's `downstream` parameter).
 * Then whenever that event is triggered during gameplay, the event trap
 * first executes all the upstream listeners according to mod priority.
 * Next, it executes the vanilla listener for that event (or the parent
 * object's corresponding event listener if undefined by the HLD developers).
 * Finally, it executes all of the downstream listeners according to
 * inverted mod priority (lowest priority mod listeners get executed first).
 *
 * This flow from upstream to vanilla to downstream may, however, be
 * interrupted. Each mod event listener returns a boolean value. If	`true`,
 * then execution continues on to the next listener in the stream. If
 * `false`, however, then all listeners downstream of the current listener
 * are bypassed (including the vanilla listener). By attaching an upstream
 * listener that always returns `false`, vanilla events can effectively be
 * "disabled," but this may break mod compatibility.
 *
 * @since 1.0.0
 */
#ifndef AER_OBJECT_H
#define AER_OBJECT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "aer/instance.h"



/* ----- PUBLIC TYPES ----- */

/**
 * @brief Vanilla objects.
 *
 * @since 1.0.0
 */
typedef enum AERObjectIndex {
	AER_OBJECT_MASTERCLASS = 0x0,
	AER_OBJECT_DOORSTOVISIT = 0x1,
	AER_OBJECT_PAUSEDELAYOBJ = 0x2,
	AER_OBJECT_ANALYTICS = 0x3,
	AER_OBJECT_BACKGROUND = 0x4,
	AER_OBJECT_STUCKBOX = 0x5,
	AER_OBJECT_BATTERYCHARGER = 0x6,
	AER_OBJECT_BG = 0x7,
	AER_OBJECT_BGSCENERY = 0x8,
	AER_OBJECT_BULLET = 0x9,
	AER_OBJECT_BURSTSHOT = 0xa,
	AER_OBJECT_CONFIRMWINDOW = 0xb,
	AER_OBJECT_DATA = 0xc,
	AER_OBJECT_DECOR = 0xd,
	AER_OBJECT_DECORSHADOW = 0xe,
	AER_OBJECT_DECORXRAYBLOCK = 0xf,
	AER_OBJECT_DECORXRAYSHADOWBLOCK = 0x10,
	AER_OBJECT_ENEMY = 0x11,
	AER_OBJECT_ENEMYHITMASK = 0x12,
	AER_OBJECT_FOREGROUND = 0x13,
	AER_OBJECT_GAMEEXITER = 0x14,
	AER_OBJECT_HAZARD = 0x15,
	AER_OBJECT_JUMPLEDGE = 0x16,
	AER_OBJECT_LIBRARY = 0x17,
	AER_OBJECT_MANTLEABLE = 0x18,
	AER_OBJECT_MOVINGBLOCK = 0x19,
	AER_OBJECT_MUSIC = 0x1a,
	AER_OBJECT_OBJWINDOW = 0x1b,
	AER_OBJECT_PARTICLE = 0x1c,
	AER_OBJECT_PATHFINDOBSTACLE = 0x1d, /**< All "solid" objects must either
																				directly or indirectly inherit from
																				this object. */
	AER_OBJECT_PROJECTILEBLOCKCOLLIDER = 0x1e,
	AER_OBJECT_SCENERY = 0x1f,
	AER_OBJECT_SCREEN = 0x20,
	AER_OBJECT_SIMPLEEFFECT = 0x21,
	AER_OBJECT_SIMPLEEFFECTGUI = 0x22,
	AER_OBJECT_STATESCENERY = 0x23,
	AER_OBJECT_RECESSINGSCENERY = 0x24,
	AER_OBJECT_TIMELIMIT = 0x25,
	AER_OBJECT_MENUS = 0x26,
	AER_OBJECT_CREDITS = 0x27,
	AER_OBJECT_TITLEHLD = 0x28,
	AER_OBJECT_PAXCHALLENGERESULTS = 0x29,
	AER_OBJECT_THANKYOUSCREEN = 0x2a,
	AER_OBJECT_TITLESCREEN = 0x2b,
	AER_OBJECT_WAITER = 0x2c,
	AER_OBJECT_BOSSRUSHLEADERBOARDSAVER = 0x2d,
	AER_OBJECT_EDITOROBJ = 0x2e,
	AER_OBJECT_SPAWNER = 0x2f,
	AER_OBJECT_LEVELBOUNDARY = 0x30,
	AER_OBJECT_FOLDER_EDITOROBJS = 0x31,
	AER_OBJECT_HOARDE = 0x32,
	AER_OBJECT_CHARMAKER = 0x33,
	AER_OBJECT_CHARVICTORY = 0x34,
	AER_OBJECT_NEWREGIONSOUNDEVENT = 0x35,
	AER_OBJECT_ALLHOARDESBEATEN = 0x36,
	AER_OBJECT_BONUSHOARDEBEATEN = 0x37,
	AER_OBJECT_DOOR = 0x38,
	AER_OBJECT_TITLEDOOR = 0x39,
	AER_OBJECT_EDITORCHECKPOINT = 0x3a,
	AER_OBJECT_REGION = 0x3b,
	AER_OBJECT_REGIONWALL = 0x3c,
	AER_OBJECT_BOSSWALL = 0x3d,
	AER_OBJECT_BOOMBOX = 0x3e,
	AER_OBJECT_AMBIENCE = 0x3f,
	AER_OBJECT_CAMERACUE = 0x40,
	AER_OBJECT_CAMERALOCK = 0x41,
	AER_OBJECT_CASERESETTER = 0x42,
	AER_OBJECT_EMPTYOBJECT = 0x43,
	AER_OBJECT_TRUEATINTERVAL = 0x44,
	AER_OBJECT_PERMASTATE = 0x45,
	AER_OBJECT_COLLECTIBLECHECK = 0x46,
	AER_OBJECT_PLAYERHASMAPCHECK = 0x47,
	AER_OBJECT_WELLCHECK = 0x48,
	AER_OBJECT_ROOMVISITED = 0x49,
	AER_OBJECT_BOSSCHECK = 0x4a,
	AER_OBJECT_ONETIMETRUE = 0x4b,
	AER_OBJECT_PARALLAXOBJ = 0x4c,
	AER_OBJECT_NOCOMBAT = 0x4d,
	AER_OBJECT_NOSHOOT = 0x4e,
	AER_OBJECT_NOWARP = 0x4f,
	AER_OBJECT_EDITORBLOOMER = 0x50,
	AER_OBJECT_EDITORSCREENSHAKE = 0x51,
	AER_OBJECT_EDITORSOUND = 0x52,
	AER_OBJECT_EDITORCUTSCENE = 0x53,
	AER_OBJECT_TUTORIALBUTTONPROMPT = 0x54,
	AER_OBJECT_TUTORIALINFINITESLIME = 0x55,
	AER_OBJECT_EDITORWAYPOINT = 0x56,
	AER_OBJECT_NOTE = 0x57,
	AER_OBJECT_SPECIALWAYPOINT = 0x58,
	AER_OBJECT_DRIFTERDEATH = 0x59,
	AER_OBJECT_GLOWINGEYES = 0x5a,
	AER_OBJECT_BLOODSPRAYER = 0x5b,
	AER_OBJECT_ENEMYHPCHECKER = 0x5c,
	AER_OBJECT_BOSSRUSH = 0x5d,
	AER_OBJECT_BOSSRUSHCHECKPOINT = 0x5e,
	AER_OBJECT_BOSSRUSHBOSSCHECK = 0x5f,
	AER_OBJECT_FOLDER_ENDE = 0x60,
	AER_OBJECT_AMBIENTSOUND = 0x61,
	AER_OBJECT_GAUNTLETDOOR = 0x62,
	AER_OBJECT_CRISSCROSS = 0x63,
	AER_OBJECT_OVERTRIGGER = 0x64,
	AER_OBJECT_UNDERTRIGGER = 0x65,
	AER_OBJECT_DOORBOTTOM = 0x66,
	AER_OBJECT_GRASS = 0x67,
	AER_OBJECT_HALSPAWNER = 0x68,
	AER_OBJECT_MIDDOOR = 0x69,
	AER_OBJECT_PUDDLE = 0x6a,
	AER_OBJECT_DESTRUCTABLE = 0x6b, /**< Objects that release energy when
																		destroyed. */
	AER_OBJECT_PHASECRYSTALMAKER = 0x6c,
	AER_OBJECT_CRYSTALDESTRUCTABLE = 0x6d,
	AER_OBJECT_MULTIHITCRYSTAL = 0x6e,
	AER_OBJECT_ORGANTUBETINY = 0x6f,
	AER_OBJECT_ORGANTUBETHIN = 0x70,
	AER_OBJECT_ORGANTUBESMALL = 0x71,
	AER_OBJECT_COLLECTIBLE = 0x72,
	AER_OBJECT_COLLECTIBLECOLUMN = 0x73,
	AER_OBJECT_FOLDER_ITEMS = 0x74,
	AER_OBJECT_CRATE = 0x75,
	AER_OBJECT_CRATEBIG = 0x76,
	AER_OBJECT_MULTIHITCRATE = 0x77,
	AER_OBJECT_BARREL = 0x78,
	AER_OBJECT_EXPLODINGBARREL = 0x79,
	AER_OBJECT_GEARBITCRATE = 0x7a,
	AER_OBJECT_GEARBIT = 0x7b,
	AER_OBJECT_MAP = 0x7c,
	AER_OBJECT_HEALTHKIT = 0x7d,
	AER_OBJECT_DRIFTERBONES_KEY = 0x7e,
	AER_OBJECT_DRIFTERBONES_WEAPON = 0x7f,
	AER_OBJECT_DRIFTERBONES_OUTFIT = 0x80,
	AER_OBJECT_MODULESOCKET = 0x81,
	AER_OBJECT_LIBRARIANTABLET = 0x82,
	AER_OBJECT_LIBRARYCASE = 0x83,
	AER_OBJECT_LIBRARYWALL = 0x84,
	AER_OBJECT_LIBRARYWALLFINAL = 0x85,
	AER_OBJECT_FOLDER_ENDD = 0x86,
	AER_OBJECT_DRIFTERBONES = 0x87,
	AER_OBJECT_BATTERYREFILLER = 0x88,
	AER_OBJECT_HEALTHPLANT = 0x89,
	AER_OBJECT_FOLDER_ENVIRO = 0x8a,
	AER_OBJECT_BUTTON = 0x8b,
	AER_OBJECT_BOSSRUSHLEADERBOARD = 0x8c,
	AER_OBJECT_HIGHSCOREBOARD = 0x8d,
	AER_OBJECT_SMALLHIGHSCOREBOARD = 0x8e,
	AER_OBJECT_TOGGLESWITCH = 0x8f,
	AER_OBJECT_RAILGUNSWITCH = 0x90,
	AER_OBJECT_TERMINAL = 0x91,
	AER_OBJECT_WARPPAD = 0x92,
	AER_OBJECT_STAIRSRIGHT = 0x93,
	AER_OBJECT_STAIRSLEFT = 0x94,
	AER_OBJECT_SMALLSTAIRSRIGHT = 0x95,
	AER_OBJECT_SMALLSTAIRSLEFT = 0x96,
	AER_OBJECT_STAIRSUP = 0x97,
	AER_OBJECT_SMALLSTAIRSUP = 0x98,
	AER_OBJECT_JUMPPAD = 0x99,
	AER_OBJECT_INVISIBLEPLATFORM = 0x9a,
	AER_OBJECT_LIGHT = 0x9b,
	AER_OBJECT_ROOMDOOR = 0x9c,
	AER_OBJECT_DIAMONDDOOR = 0x9d,
	AER_OBJECT_VANISHINGDOOR = 0x9e,
	AER_OBJECT_TELEVATOR = 0x9f,
	AER_OBJECT_BIGBOSSDOOR = 0xa0,
	AER_OBJECT_DRIFTERVAULTDOOR = 0xa1,
	AER_OBJECT_MODULEDOOR = 0xa2,
	AER_OBJECT_SHORTWARP = 0xa3,
	AER_OBJECT_TELEPORTER = 0xa4,
	AER_OBJECT_UPGRADESWORD = 0xa5,
	AER_OBJECT_UPGRADEDASH = 0xa6,
	AER_OBJECT_UPGRADEHEALTHPACK = 0xa7,
	AER_OBJECT_UPGRADESPECIAL = 0xa8,
	AER_OBJECT_UPGRADEWEAPON = 0xa9,
	AER_OBJECT_APARTMENTDIAGRAM = 0xaa,
	AER_OBJECT_APARTMENTLIGHTSWITCH = 0xab,
	AER_OBJECT_APARTMENTMIRROR = 0xac,
	AER_OBJECT_CAPECHOOSER = 0xad,
	AER_OBJECT_COMPANIONSHELLCHOOSER = 0xae,
	AER_OBJECT_SWORDCHOOSER = 0xaf,
	AER_OBJECT_STAMINARECHARGER = 0xb0,
	AER_OBJECT_CHAINDASHSCOREBOARD = 0xb1,
	AER_OBJECT_CHAINDASHSCOREBOARDPRO = 0xb2,
	AER_OBJECT_WELLTOWER = 0xb3,
	AER_OBJECT_WATERFLOOR = 0xb4,
	AER_OBJECT_SNOWFLOOR = 0xb5,
	AER_OBJECT_WATERFALLREGION = 0xb6,
	AER_OBJECT_TITANHEART = 0xb7,
	AER_OBJECT_TITANEYE = 0xb8,
	AER_OBJECT_GOOCIRCLE = 0xb9,
	AER_OBJECT_SOCCERBALL = 0xba,
	AER_OBJECT_SOCCERSCOREBOARD = 0xbb,
	AER_OBJECT_ABYSSDOOR = 0xbc,
	AER_OBJECT_ABYSSDOORPILLAR = 0xbd,
	AER_OBJECT_FOLDER_END3 = 0xbe,
	AER_OBJECT_SNOWPILE = 0xbf,
	AER_OBJECT_VINEBOTTOM = 0xc0,
	AER_OBJECT_INTERACTIVEOBJ = 0xc1,
	AER_OBJECT_SCENERYLIGHT = 0xc2,
	AER_OBJECT_DOORINTERACTIVESYSTEM = 0xc3,
	AER_OBJECT_SWITCHPARENT = 0xc4,
	AER_OBJECT_FLOORBUTTON = 0xc5,
	AER_OBJECT_SWITCHPILLAR = 0xc6,
	AER_OBJECT_TIMEDSWITCHPILLAR = 0xc7,
	AER_OBJECT_SLOWSWITCH = 0xc8,
	AER_OBJECT_SPLITTINGDOOR = 0xc9,
	AER_OBJECT_SINKINGDOOR = 0xca,
	AER_OBJECT_TOWER = 0xcb,
	AER_OBJECT_DANGER = 0xcc,
	AER_OBJECT_ACIDPARENT = 0xcd,
	AER_OBJECT_LASERHAZARD = 0xce,
	AER_OBJECT_ACIDPOOL = 0xcf,
	AER_OBJECT_ACIDPOOLSHRINK = 0xd0,
	AER_OBJECT_FOLDER_HAZARDS = 0xd1,
	AER_OBJECT_POPUPTURRET = 0xd2,
	AER_OBJECT_CRUSHBLOCK = 0xd3,
	AER_OBJECT_MOVINGPLATFORM = 0xd4,
	AER_OBJECT_RISINGPLATFORM = 0xd5,
	AER_OBJECT_DROPPLATFORM = 0xd6,
	AER_OBJECT_SICKAREA = 0xd7,
	AER_OBJECT_PHASEMINE = 0xd8,
	AER_OBJECT_FLAMEPOLE = 0xd9,
	AER_OBJECT_FLAMEVENT = 0xda,
	AER_OBJECT_FLAMETHROWER = 0xdb,
	AER_OBJECT_LINKMINE = 0xdc,
	AER_OBJECT_FOLDER_ENDH = 0xdd,
	AER_OBJECT_SINKINGPLATFORM = 0xde,
	AER_OBJECT_SHALLOWPLATFORM = 0xdf,
	AER_OBJECT_WARPBLOCKTRAP = 0xe0,
	AER_OBJECT_TIMEPOLE = 0xe1,
	AER_OBJECT_TURRET = 0xe2,
	AER_OBJECT_TIMESLOWER = 0xe3,
	AER_OBJECT_FLAMEJET = 0xe4,
	AER_OBJECT_TURRETLASER = 0xe5,
	AER_OBJECT_TURRETORB = 0xe6,
	AER_OBJECT_PHASEDROPPLATFORM = 0xe7,
	AER_OBJECT_MINE = 0xe8,
	AER_OBJECT_OTTERBODY = 0xe9,
	AER_OBJECT_ACIDLAKE = 0xea,
	AER_OBJECT_PHASEPLATFORM = 0xeb,
	AER_OBJECT_DESTRUCTOR = 0xec,
	AER_OBJECT_ENEMYSHOUT = 0xed,
	AER_OBJECT_OTHERCOMPANION = 0xee,
	AER_OBJECT_ENEMYBOMB = 0xef,
	AER_OBJECT_GOALBOMB = 0xf0,
	AER_OBJECT_CULTCHARGE = 0xf1,
	AER_OBJECT_ENEMYWEAPONCOL = 0xf2,
	AER_OBJECT_CRYSTALSPIKE = 0xf3,
	AER_OBJECT_TIMEPHASEBULLET = 0xf4,
	AER_OBJECT_NINJASTAR = 0xf5,
	AER_OBJECT_LASERSHOT = 0xf6,
	AER_OBJECT_ENEMYSHOT = 0xf7,
	AER_OBJECT_ROCKET = 0xf8,
	AER_OBJECT_ENEMYBULLET = 0xf9,
	AER_OBJECT_MAGICMISSILE = 0xfa,
	AER_OBJECT_HALDRIFTER = 0xfb,
	AER_OBJECT_DEADHALLUCINATION = 0xfc,
	AER_OBJECT_HALDIRK = 0xfd,
	AER_OBJECT_ENEMYPLOPPER = 0xfe,
	AER_OBJECT_TESTENEMY = 0xff,
	AER_OBJECT_PARRYPRINCE = 0x100,
	AER_OBJECT_TANUKISPEAR = 0x101,
	AER_OBJECT_CRYSTALSPIDER = 0x102,
	AER_OBJECT_ALPHAWOLF = 0x103,
	AER_OBJECT_NINJAFROG = 0x104,
	AER_OBJECT_JARFROG = 0x105,
	AER_OBJECT_STRIDER = 0x106,
	AER_OBJECT_SWOOPNSPIT = 0x107,
	AER_OBJECT_GARBAGEPLANT = 0x108,
	AER_OBJECT_DROPBIRD = 0x109,
	AER_OBJECT_DIVEBOMB = 0x10a,
	AER_OBJECT_CRAB = 0x10b,
	AER_OBJECT_CRABMAN = 0x10c,
	AER_OBJECT_BURSTBIRD = 0x10d,
	AER_OBJECT_SUMMONBLOCKBIRD = 0x10e,
	AER_OBJECT_FOLDER_ENEMYG = 0x10f,
	AER_OBJECT_DIRK = 0x110,
	AER_OBJECT_RIFLEDIRK = 0x111,
	AER_OBJECT_MISSILEDIRK = 0x112,
	AER_OBJECT_SLIME = 0x113,
	AER_OBJECT_LEAPER = 0x114,
	AER_OBJECT_SPIDER = 0x115,
	AER_OBJECT_DIRKOMMANDER = 0x116,
	AER_OBJECT_PUNCHINGBAG = 0x117,
	AER_OBJECT_WEAKPUNCHINGBAG = 0x118,
	AER_OBJECT_REGPUNCHINGBAG = 0x119,
	AER_OBJECT_FOLDER_ENEMYW = 0x11a,
	AER_OBJECT_TANUKISWORD = 0x11b,
	AER_OBJECT_TANUKIGUN = 0x11c,
	AER_OBJECT_SMALLCRYSTALSPIDER = 0x11d,
	AER_OBJECT_CRYSTALBABY = 0x11e,
	AER_OBJECT_WOLF = 0x11f,
	AER_OBJECT_FOLDER_ENEMYE = 0x120,
	AER_OBJECT_NINJASTARFROG = 0x121,
	AER_OBJECT_SPIRALBOMBFROG = 0x122,
	AER_OBJECT_GRUMPSHROOM = 0x123,
	AER_OBJECT_MELTY = 0x124,
	AER_OBJECT_FOLDER_ENEMYN = 0x125,
	AER_OBJECT_GHOSTBEAMBIRD = 0x126,
	AER_OBJECT_CULTBIRD = 0x127,
	AER_OBJECT_BIRDMAN = 0x128,
	AER_OBJECT_FOLDER_ENEMYS = 0x129,
	AER_OBJECT_SOUTHDRONE = 0x12a,
	AER_OBJECT_ROBODOG = 0x12b,
	AER_OBJECT_BLADIRK = 0x12c,
	AER_OBJECT_FOLDER_END = 0x12d,
	AER_OBJECT_FOLDER_BOSSES = 0x12e,
	AER_OBJECT_HALBOSS = 0x12f,
	AER_OBJECT_CLEANER = 0x130,
	AER_OBJECT_OLDGENERAL = 0x131,
	AER_OBJECT_JERKPOPE = 0x132,
	AER_OBJECT_MARKSCYTHE = 0x133,
	AER_OBJECT_BENNYARROW = 0x134,
	AER_OBJECT_BULLETBAKER = 0x135,
	AER_OBJECT_ALUCARDMODULESOCKET = 0x136,
	AER_OBJECT_COUNTALUCARD = 0x137,
	AER_OBJECT_TANUKICRYSTAL = 0x138,
	AER_OBJECT_FOLDER_END5 = 0x139,
	AER_OBJECT_GEARBITSPAWNER = 0x13a,
	AER_OBJECT_HALEXPLOSION = 0x13b,
	AER_OBJECT_HALARM = 0x13c,
	AER_OBJECT_DIAMONDEYE = 0x13d,
	AER_OBJECT_EMBERDIAMOND = 0x13e,
	AER_OBJECT_EMBERARM = 0x13f,
	AER_OBJECT_EMBERARMPIECE = 0x140,
	AER_OBJECT_EMBERLASER = 0x141,
	AER_OBJECT_GUNSLINGER = 0x142,
	AER_OBJECT_CRYSTALQUEEN = 0x143,
	AER_OBJECT_SPIDEREGG = 0x144,
	AER_OBJECT_ALUCARDDRONE = 0x145,
	AER_OBJECT_ARROWSTRIKE = 0x146,
	AER_OBJECT_SCYTHE = 0x147,
	AER_OBJECT_WARPTRAVELER = 0x148,
	AER_OBJECT_DIAMONDSPIDER = 0x149,
	AER_OBJECT_CRYSTALSPIKEMAKER = 0x14a,
	AER_OBJECT_CRYSTALWALLMAKER = 0x14b,
	AER_OBJECT_DIAMONDSPIDERLEG = 0x14c,
	AER_OBJECT_BOSSGEARBITSPAWNER = 0x14d,
	AER_OBJECT_BLOODPICKUP = 0x14e,
	AER_OBJECT_COMPANIONSUIT = 0x14f,
	AER_OBJECT_BETACAPEPICKUP = 0x150,
	AER_OBJECT_POWERUP = 0x151,
	AER_OBJECT_POWDRONE = 0x152,
	AER_OBJECT_POWBUBBLEDRONE = 0x153,
	AER_OBJECT_POWSTUNDRONE = 0x154,
	AER_OBJECT_FOLDER_NPC = 0x155,
	AER_OBJECT_WAYPOINT = 0x156,
	AER_OBJECT_LIZARDFAMILY = 0x157,
	AER_OBJECT_CHANTBIRD = 0x158,
	AER_OBJECT_NPCGENERIC = 0x159,
	AER_OBJECT_NPCALTDRIFTER = 0x15a,
	AER_OBJECT_NPC = 0x15b,
	AER_OBJECT_CITIZEN = 0x15c,
	AER_OBJECT_BUFFALO = 0x15d,
	AER_OBJECT_BADASSDRIFTER = 0x15e,
	AER_OBJECT_BADASSINAPARTMENT = 0x15f,
	AER_OBJECT_BADASSINOFFICE = 0x160,
	AER_OBJECT_FOLDER_END2 = 0x161,
	AER_OBJECT_NPCCUSTOMBASE = 0x162,
	AER_OBJECT_FOLDER_WILDLIFE = 0x163,
	AER_OBJECT_DIRKGOALIE = 0x164,
	AER_OBJECT_EMBERFOX = 0x165,
	AER_OBJECT_ABYSSDOG = 0x166,
	AER_OBJECT_JARFROGCARRY = 0x167,
	AER_OBJECT_FROGDRAGOTTER = 0x168,
	AER_OBJECT_TADPOLE = 0x169,
	AER_OBJECT_FLY = 0x16a,
	AER_OBJECT_TINYFLY = 0x16b,
	AER_OBJECT_EEL = 0x16c,
	AER_OBJECT_SNAIL = 0x16d,
	AER_OBJECT_BIRDMANFLYAWAY = 0x16e,
	AER_OBJECT_FISH = 0x16f,
	AER_OBJECT_DOG = 0x170,
	AER_OBJECT_SQUIRREL = 0x171,
	AER_OBJECT_DEER = 0x172,
	AER_OBJECT_ROBIN = 0x173,
	AER_OBJECT_CROW = 0x174,
	AER_OBJECT_HALBIRD = 0x175,
	AER_OBJECT_HERON = 0x176,
	AER_OBJECT_WILDDRONE = 0x177,
	AER_OBJECT_SQUIDBOT = 0x178,
	AER_OBJECT_SCORPBOT = 0x179,
	AER_OBJECT_BIRD = 0x17a,
	AER_OBJECT_BIRDBLACK = 0x17b,
	AER_OBJECT_FOLDER_ENDW = 0x17c,
	AER_OBJECT_WILDLIFE = 0x17d,
	AER_OBJECT_WEATHEROBJ = 0x17e,
	AER_OBJECT_BLACKRAIN = 0x17f,
	AER_OBJECT_FOLDER_FX = 0x180,
	AER_OBJECT_COLORREGION = 0x181,
	AER_OBJECT_SCREENDUST = 0x182,
	AER_OBJECT_TINYDUST = 0x183,
	AER_OBJECT_RAINBOWGLITTER = 0x184,
	AER_OBJECT_PULSER = 0x185,
	AER_OBJECT_WATERSPARKLE = 0x186,
	AER_OBJECT_RAINBOWBGDUST = 0x187,
	AER_OBJECT_SNOWMAKER = 0x188,
	AER_OBJECT_SLOWSNOW = 0x189,
	AER_OBJECT_RAINMAKER = 0x18a,
	AER_OBJECT_LEAKYRAIN = 0x18b,
	AER_OBJECT_LEAKYRAINPART = 0x18c,
	AER_OBJECT_BLACKRAINMAKER = 0x18d,
	AER_OBJECT_CLOUDS = 0x18e,
	AER_OBJECT_FGMIST = 0x18f,
	AER_OBJECT_LIGHTNING = 0x190,
	AER_OBJECT_FOLDER_ENDWEATH = 0x191,
	AER_OBJECT_CLOUDWRAP = 0x192,
	AER_OBJECT_RAIN = 0x193,
	AER_OBJECT_SNOW = 0x194,
	AER_OBJECT_DUSTPART = 0x195,
	AER_OBJECT_DASHBLOCK = 0x196,
	AER_OBJECT_BLOCK = 0x197,
	AER_OBJECT_SOFTBLOCK = 0x198,
	AER_OBJECT_BLOCK2 = 0x199,
	AER_OBJECT_BLOCKPATHFINDLESS = 0x19a,
	AER_OBJECT_PARTBLOCK = 0x19b,
	AER_OBJECT_OVERWALL = 0x19c,
	AER_OBJECT_UNDERWALL = 0x19d,
	AER_OBJECT_SAFEPLATFORM = 0x19e,
	AER_OBJECT_PROTECTIONPLATFORM = 0x19f,
	AER_OBJECT_SECONDARY = 0x1a0,
	AER_OBJECT_ROOMMAPDATA = 0x1a1,
	AER_OBJECT_DRAWPOINT = 0x1a2,
	AER_OBJECT_DRAWLINE = 0x1a3,
	AER_OBJECT_DRAWCIRCLE = 0x1a4,
	AER_OBJECT_DRAWOVAL = 0x1a5,
	AER_OBJECT_DRAWTEXT = 0x1a6,
	AER_OBJECT_ATTACKCOL = 0x1a7, /**< Hitmask of attack when player swings
																	sword. */
	AER_OBJECT_HITCOLLIDER = 0x1a8,
	AER_OBJECT_CHAR = 0x1a9, /**< Player character. */
	AER_OBJECT_DECOY = 0x1aa,
	AER_OBJECT_PHANTOMSLASH = 0x1ab,
	AER_OBJECT_CHARHITMASK = 0x1ac,
	AER_OBJECT_DEADCHAR = 0x1ad,
	AER_OBJECT_DEADP2 = 0x1ae,
	AER_OBJECT_GHOST = 0x1af,
	AER_OBJECT_ALTCOMPANION = 0x1b0,
	AER_OBJECT_ORB = 0x1b1,
	AER_OBJECT_ROLLYPOLLY = 0x1b2,
	AER_OBJECT_ROLLYROCKET = 0x1b3,
	AER_OBJECT_CRYSTALLANCE = 0x1b4,
	AER_OBJECT_CRYSTALMARKER = 0x1b5,
	AER_OBJECT_CRYSTALMAKER = 0x1b6,
	AER_OBJECT_CRYSTAL = 0x1b7,
	AER_OBJECT_DIAMONDBULLET = 0x1b8,
	AER_OBJECT_BYUUBLOCKCOLLIDER = 0x1b9,
	AER_OBJECT_BOMB = 0x1ba,
	AER_OBJECT_LEAPFLAME = 0x1bb,
	AER_OBJECT_RAILLASER = 0x1bc,
	AER_OBJECT_CANNONCHUNK = 0x1bd,
	AER_OBJECT_WARPHAMMER = 0x1be,
	AER_OBJECT_HOLOAIMER = 0x1bf,
	AER_OBJECT_CHARFIRESHADOW = 0x1c0,
	AER_OBJECT_BLASTSHOCKWAVE = 0x1c1,
	AER_OBJECT_BLAST = 0x1c2,
	AER_OBJECT_BOUNCESPARK = 0x1c3,
	AER_OBJECT_DROPLET = 0x1c4,
	AER_OBJECT_DUSTPILLAR = 0x1c5,
	AER_OBJECT_FADEDEBRI = 0x1c6,
	AER_OBJECT_BREAKDEBRI = 0x1c7,
	AER_OBJECT_SPINDEBRI = 0x1c8,
	AER_OBJECT_FIRE = 0x1c9,
	AER_OBJECT_FLAG = 0x1ca,
	AER_OBJECT_FOOTPRINT = 0x1cb,
	AER_OBJECT_GRASSCLIPPING = 0x1cc,
	AER_OBJECT_IMAGEFADE = 0x1cd,
	AER_OBJECT_IMAGESTAY = 0x1ce,
	AER_OBJECT_IMAGEPART = 0x1cf,
	AER_OBJECT_MEATBLAST = 0x1d0,
	AER_OBJECT_SHATTER = 0x1d1,
	AER_OBJECT_SNAPFX = 0x1d2,
	AER_OBJECT_SPARKFLASH = 0x1d3,
	AER_OBJECT_SPARKSHOWER = 0x1d4,
	AER_OBJECT_SPLASH = 0x1d5,
	AER_OBJECT_TELEPORTERFX = 0x1d6,
	AER_OBJECT_BLOODPART = 0x1d7,
	AER_OBJECT_BLOODSPRAY = 0x1d8,
	AER_OBJECT_BLOODSTAIN = 0x1d9,
	AER_OBJECT_VIRUSSTAIN = 0x1da,
	AER_OBJECT_GIBLET = 0x1db,
	AER_OBJECT_FRIEDGIB = 0x1dc,
	AER_OBJECT_TELEPORTFX = 0x1dd,
	AER_OBJECT_GOGGLETRAIL = 0x1de,
	AER_OBJECT_DASHDIAMOND = 0x1df,
	AER_OBJECT_BURNTGROUND = 0x1e0,
	AER_OBJECT_DEATHDUST = 0x1e1,
	AER_OBJECT_SMOKESMALL = 0x1e2,
	AER_OBJECT_SMOKE = 0x1e3,
	AER_OBJECT_ROCKETSPLIT = 0x1e4,
	AER_OBJECT_EXPLOSIONPARTICLE = 0x1e5,
	AER_OBJECT_DUSTDASH = 0x1e6,
	AER_OBJECT_MISSILESMOKE = 0x1e7,
	AER_OBJECT_PARRYFLASH = 0x1e8,
	AER_OBJECT_RUNDUST = 0x1e9,
	AER_OBJECT_SHIELDDUST = 0x1ea,
	AER_OBJECT_IMPACTDUST = 0x1eb,
	AER_OBJECT_SMOKESPAWN = 0x1ec,
	AER_OBJECT_DUSTBALL = 0x1ed,
	AER_OBJECT_SLIMETRAIL = 0x1ee,
	AER_OBJECT_SLIMEPART = 0x1ef,
	AER_OBJECT_TWOFRAMEDEATH = 0x1f0,
	AER_OBJECT_BIRDMANTUMBLE = 0x1f1,
	AER_OBJECT_MUZZLEFLASH = 0x1f2,
	AER_OBJECT_ZELISKAEMMITTER = 0x1f3,
	AER_OBJECT_RAILLASERFADE = 0x1f4,
	AER_OBJECT_SHIELDPUSHSMALL = 0x1f5,
	AER_OBJECT_SHIELDPUSHMEDIUM = 0x1f6,
	AER_OBJECT_SHIELDPUSHLARGE = 0x1f7,
	AER_OBJECT_MASTERMUFFLER = 0x1f8,
	AER_OBJECT_XPSOUND = 0x1f9,
	AER_OBJECT_MISSILEHALF = 0x1fa,
	AER_OBJECT_FIREONGROUND = 0x1fb,
	AER_OBJECT_EXECUTEDELAYEDOBJ = 0x1fc,
	AER_OBJECT_OBJCTTVIDEOPLAYER = 0x1fd
} AERObjectIndex;



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Register a custom object.
 *
 * @param[in] name Name of new object.
 * @param[in] parentIdx Object from which to inherit.
 * @param[in] spriteIdx Default sprite for instances of this object.
 * @param[in] maskIdx Default collision mask for instances of this object.
 * @param[in] depth Default render depth for instances of this object.
 * @param[in] visible Default visibility for instances of this object.
 * @param[in] collisions Whether or not collision checking is enabled for all
 * instances of this object.
 * @param[in] persistent Default persistence for instances of this object.
 *
 * @return Index of new object or `-1` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside object registration stage.
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `parentIdx` is an invalid object.
 * @throw ::AER_OUT_OF_MEM if space for new object could not be allocated.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjects
 */
int32_t AERObjectRegister(
		const char * name,
		int32_t parentIdx,
		int32_t spriteIdx,
		int32_t maskIdx,
		int32_t depth,
		bool visible,
		bool collisions,
		bool persistent
);

/**
 * @brief Query the total number of vanilla and mod objects registered.
 *
 * @return Number of objects.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
size_t AERObjectGetNumRegistered(void);

/**
 * @brief Query the name of an object.
 *
 * @param[in] objIdx Object of interest.
 *
 * @return Name of object.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 */
const char * AERObjectGetName(int32_t objIdx);

/**
 * @brief Query the parent of an object.
 *
 * @param[in] objIdx Object of interest.
 *
 * @return Parent object's index.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 */
int32_t AERObjectGetParent(int32_t objIdx);

/**
 * @brief Query all instances of an object in the current room.
 *
 * @warning Does **not** include instances of child objects.
 * @warning Argument `instBuf` must be large enough to hold at least
 * `bufSize` elements.
 *
 * @note Argument `bufSize` may be `0` in which case argument `instBuf` may
 * be `NULL`. This may be used to efficiently query the total number of
 * instances of an object in the current room.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] bufSize Maximum number of elements to write to argument
 * `instBuf`.
 * @param[out] instBuf Buffer to write instances to.
 *
 * @return Total number of instances of object in current room.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `instBuf` is `NULL` and argument
 * `bufSize` is greater than `0`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 */
size_t AERObjectGetInstances(
		int32_t objIdx,
		size_t bufSize,
		AERInstance ** instBuf
);

/**
 * @brief Query whether or not an object has collision checking enabled.
 *
 * @param[in] objIdx Object of interest.
 *
 * @return Whether object has collision checking enabled.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 */
bool AERObjectGetCollisions(int32_t objIdx);

/**
 * @brief Set whether or not an object has collision checking enabled.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] collisions Whether or not to enable collision checking.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 */
void AERObjectSetCollisions(
		int32_t objIdx,
		bool collisions
);

/**
 * @brief Attach a creation event listener to an object.
 *
 * This event listener is called whenever an instance of the object
 * is created.
 *
 * @note There are certain conditions under which an instance may be
 * created without triggering a creation event.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 * @sa AERInstanceCreate
 */
void AERObjectAttachCreateListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @brief Attach a destruction event listener to an object.
 *
 * This event listener is called whenever an instance of the object
 * is destroyed.
 *
 * @note There are certain conditions under which an instance may be
 * destroyed without triggering a destruction event.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 * @sa AERInstanceDestroy
 * @sa AERInstanceDelete
 */
void AERObjectAttachDestroyListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @brief Attach an alarm event listener to an object.
 *
 * This event listener is called whenever the target alarm of an
 * instance of the object reaches `0` (after which the alarm will be set to
 * `-1`, disabling it until manually set again).
 *
 * @note It is considered good practice to make the argument `alarmIdx`
 * user-configurable using envconf.h.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] alarmIdx Alarm to watch.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object or if
 * argument `alarmIdx` is not on the interval [0, 11].
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 * @sa AERInstanceGetAlarm
 * @sa AERInstanceSetAlarm
 */
void AERObjectAttachAlarmListener(
		int32_t objIdx,
		uint32_t alarmIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @brief Attach a step event listener to an object.
 *
 * This event listener is called once in the middle of every step
 * for each instance of the object.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 */
void AERObjectAttachStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @brief Attach a pre-step event listener to an object.
 *
 * This event listener is called once at the start of every step
 * for each instance of the object.
 *
 * @note The listener is called *after* AERModDef::roomStepListener.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 */
void AERObjectAttachPreStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @brief Attach a post-step event listener to an object.
 *
 * This event listener is called once at the end of every step
 * for each instance of the object.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 */
void AERObjectAttachPostStepListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);

/**
 * @brief Attach a collision event listener to an object.
 *
 * This event listener is called when a collision occurs between 
 * instances of the target object and the other object.
 *
 * @note In order for the listener to be called, **both** the target object and
 * the other object must have collisions enabled.
 *
 * @param[in] targetObjIdx Object of interest.
 * @param[in] otherObjIdx Other object.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `targetObjIdx` or `otherObjIdx`
 * are invalid objects.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 * @sa AERObjectGetCollisions
 * @sa AERObjectSetCollisions
 */
void AERObjectAttachCollisionListener(
		int32_t targetObjIdx,
		int32_t otherObjIdx,
		bool (* listener)(AERInstance * target, AERInstance * other),
		bool downstream
);

/**
 * @brief Attach an animation-end event listener to an object.
 *
 * This event listener is called whenever the animation frame of an instance
 * of the object loops back to `0`.
 *
 * @param[in] objIdx Object of interest.
 * @param[in] listener Callback function executed when target event occurs.
 * For more information see @ref ObjListeners.
 * @param[in] downstream If `true`, the listener is called *after* the
 * vanilla listener for the target event. Otherwise it is called before.
 *
 * @throw ::AER_SEQ_BREAK if called outside listener registration stage.
 * @throw ::AER_NULL_ARG if argument `listener` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if argument `objIdx` is an invalid object.
 *
 * @since 1.0.0
 *
 * @sa AERModDef::regObjListeners
 * @sa AERInstanceGetSpriteFrame
 * @sa AERInstanceSetSpriteFrame
 * @sa AERInstanceGetSpriteSpeed
 * @sa AERInstanceSetSpriteSpeed
 */
void AERObjectAttachAnimationEndListener(
		int32_t objIdx,
		bool (* listener)(AERInstance * inst),
		bool downstream
);



#endif /* AER_OBJECT_H */
