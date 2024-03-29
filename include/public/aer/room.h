/**
 * @file
 *
 * @brief Utilities for querying and manipulating game rooms.
 *
 * @since 1.0.0
 *
 * @copyright 2021 the libaermre authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef AER_ROOM_H
#define AER_ROOM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "aer/instance.h"

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Vanilla rooms.
 *
 * @since 1.0.0
 */
typedef enum AERRoomIndex {
    /**
     * @brief Flag which represents either no room or an invalid room depending
     * on context.
     */
    AER_ROOM_NULL = -1,
    AER_ROOM__INIT = 0x0,
    AER_ROOM_FOLDER_MENU = 0x1,
    AER_ROOM_AUTOSAVEMESSAGE = 0x2,
    AER_ROOM_CONTROLLER = 0x3,
    AER_ROOM_HEARTMACHINE = 0x4,
    AER_ROOM_TITLE = 0x5,
    AER_ROOM_PAX_CHALLENGERESULTS = 0x6,
    AER_ROOM_THANKYOU = 0x7,
    AER_ROOM_CREDITS = 0x8,
    AER_ROOM_EXITGAME = 0x9,
    AER_ROOM_WIPWARNING = 0xa,
    AER_ROOM_FOLDER_ENDMENU = 0xb,
    AER_ROOM_FOLDER_TEST = 0xc,
    AER_ROOM_GENERAL_TEST = 0xd,
    AER_ROOM_0BEAUSPLAYGROUND = 0xe,
    AER_ROOM_0TEDDYSPLAYGROUND = 0xf,
    AER_ROOM_0AKASHPLAYGROUND = 0x10,
    AER_ROOM_0ALXSPLAYGROUND = 0x11,
    AER_ROOM_0CASEYPLAYGROUND = 0x12,
    AER_ROOM_LISATEST = 0x13,
    AER_ROOM_BATTLEROOM02 = 0x14,
    AER_ROOM_HOARDE = 0x15,
    AER_ROOM_STAGINGAREA02 = 0x16,
    AER_ROOM_ZCOLUMNRISE = 0x17,
    AER_ROOM_ENEMYMUSEUM = 0x18,
    AER_ROOM_FOLDER_ENDTEST = 0x19,
    AER_ROOM_FOLDER_CHALLENGES = 0x1a,
    AER_ROOM_____MISC = 0x1b,
    AER_ROOM_PREGAUNTLETLUSHBIG02 = 0x1c,
    AER_ROOM_GAUNTLETLOADOUT = 0x1d,
    AER_ROOM_CH_GAUNTLETOPENING = 0x1e,
    AER_ROOM_CH_GAUNTLETEND = 0x1f,
    AER_ROOM_____GAUNTLETOLD = 0x20,
    AER_ROOM_CH_A_TESTY = 0x21,
    AER_ROOM_CH_ASLASHDASH = 0x22,
    AER_ROOM_CH_BDND = 0x23,
    AER_ROOM_CH_BPUNCHBAGPANIC = 0x24,
    AER_ROOM_CH_BTBESTLEVEL = 0x25,
    AER_ROOM_CH_CSHOOTINGGALLERY = 0x26,
    AER_ROOM_CH_CTESTING = 0x27,
    AER_ROOM_CH_TRIFLECIRCUS = 0x28,
    AER_ROOM_CH_TSUPERCRATEBLOX = 0x29,
    AER_ROOM_GAUNTLETCOLLECTABLE = 0x2a,
    AER_ROOM_PREGAUNTLETLUSH = 0x2b,
    AER_ROOM_FOLDER_ENDCHALLENGES = 0x2c,
    AER_ROOM_FOLDER_INTRO = 0x2d,
    AER_ROOM_IN_01_BROKENSHALLOWS = 0x2e,
    AER_ROOM_IN_02_TUTORIAL = 0x2f,
    AER_ROOM_IN_03_TUT_COMBAT = 0x30,
    AER_ROOM_IN_HORIZONCLIFF = 0x31,
    AER_ROOM_IN_HALUCINATIONDEATH = 0x32,
    AER_ROOM_IN_DRIFTERFIRE = 0x33,
    AER_ROOM_IN_BLACKWAITROOM = 0x34,
    AER_ROOM_IN_BACKERTABLET = 0x35,
    AER_ROOM_____OLD_INTRO = 0x36,
    AER_ROOM_INL_SECRETS = 0x37,
    AER_ROOM_LIN_GAPS = 0x38,
    AER_ROOM_LIN_COMBAT = 0x39,
    AER_ROOM_FOLDER_ENDINTRO = 0x3a,
    AER_ROOM_FOLDER_CENTRAL = 0x3b,
    AER_ROOM_C_DRIFTERWORKSHOP = 0x3c,
    AER_ROOM_C_CENTRAL = 0x3d,
    AER_ROOM_C_DREGS_N = 0x3e,
    AER_ROOM_C_DREGS_S = 0x3f,
    AER_ROOM_C_DREGS_E = 0x40,
    AER_ROOM_C_DREGS_W = 0x41,
    AER_ROOM_C_VEN_APOTH = 0x42,
    AER_ROOM_C_VEN_DASH = 0x43,
    AER_ROOM_C_VEN_GUN = 0x44,
    AER_ROOM_C_VEN_SPEC = 0x45,
    AER_ROOM_C_VEN_SDOJO = 0x46,
    AER_ROOM_CARENA = 0x47,
    AER_ROOM_PAX_STAGING = 0x48,
    AER_ROOM_PAX_ARENA1 = 0x49,
    AER_ROOM_PAX_ARENA2 = 0x4a,
    AER_ROOM_PAX_ARENAE = 0x4b,
    AER_ROOM_PAX_ARENAW = 0x4c,
    AER_ROOM_PAX_ARENAALL = 0x4d,
    AER_ROOM_C_BACKERTABLETX = 0x4e,
    AER_ROOM_TELEVATORSHAFT = 0x4f,
    AER_ROOM______OLD = 0x50,
    AER_ROOM_Z_FOLDER_ENDCENTRAL = 0x51,
    AER_ROOM_FOLDER_NORTH = 0x52,
    AER_ROOM_____WINDING_RIDGE = 0x53,
    AER_ROOM_NL_ENTRANCEPATH = 0x54,
    AER_ROOM_NX_TITANVISTA = 0x55,
    AER_ROOM_NX_NORTHHALL = 0x56,
    AER_ROOM_NL_CAVEVAULT = 0x57,
    AER_ROOM_NX_AFTERTITAN = 0x58,
    AER_ROOM_NC_NPCHATCHERY = 0x59,
    AER_ROOM_NX_SHRINEPATH = 0x5a,
    AER_ROOM_NL_SHRINEPATH2VAULT = 0x5b,
    AER_ROOM_NX_CAVE01 = 0x5c,
    AER_ROOM_NX_SHRINEPATH_2 = 0x5d,
    AER_ROOM_NX_MOONCOURTYARD = 0x5e,
    AER_ROOM_NX_TOWERLOCK = 0x5f,
    AER_ROOM_NC_CLIFFCAMPFIRE = 0x60,
    AER_ROOM_NL_TOBROKENSHALLOWS = 0x61,
    AER_ROOM_NX_STAIRS03 = 0x62,
    AER_ROOM_____WARP_CRUSH = 0x63,
    AER_ROOM_NL_WARPROOM = 0x64,
    AER_ROOM_NL_CRUSHWARPHALL = 0x65,
    AER_ROOM_NL_CRUSHTRANSITION = 0x66,
    AER_ROOM_NL_CRUSHBACKLOOP = 0x67,
    AER_ROOM_NC_CRUSHARENA = 0x68,
    AER_ROOM_____CULT_DROP = 0x69,
    AER_ROOM_NL_DROPSPIRALOPEN = 0x6a,
    AER_ROOM_NL_DROPPITS = 0x6b,
    AER_ROOM_NL_DROPBLOCKCULTFIGHT = 0x6c,
    AER_ROOM_NL_DROPARENA = 0x6d,
    AER_ROOM_____RISING_GAP = 0x6e,
    AER_ROOM_NL_GAPOPENING = 0x6f,
    AER_ROOM_NX_GAPWIDE = 0x70,
    AER_ROOM_NL_GAPHALLWAY = 0x71,
    AER_ROOM_NL_RISINGARENA = 0x72,
    AER_ROOM_____POPE_CATHEDRAL = 0x73,
    AER_ROOM_NX_CATHEDRALENTRANCE = 0x74,
    AER_ROOM_NX_CATHEDRALHALL = 0x75,
    AER_ROOM_NL_ALTARTHRONE = 0x76,
    AER_ROOM_NX_SPIRALSTAIRCASE = 0x77,
    AER_ROOM_NX_LIBRARIANTABLET = 0x78,
    AER_ROOM_NX_JERKPOPE = 0x79,
    AER_ROOM_____OLD_NORTH = 0x7a,
    AER_ROOM_NL_STAIRASCENT = 0x7b,
    AER_ROOM_NL_CRUSHARENA = 0x7c,
    AER_ROOM_FOLDER_ENDNORTH = 0x7d,
    AER_ROOM_FOLDER_SOUTH = 0x7e,
    AER_ROOM_____SURFACE_SOUTH = 0x7f,
    AER_ROOM_SX_SOUTHOPENING = 0x80,
    AER_ROOM_CH_CTEMPLATE = 0x81,
    AER_ROOM_SX_TOWERSOUTH = 0x82,
    AER_ROOM_SX_NPC = 0x83,
    AER_ROOM_S_GAUNTLET_ELEVATOR = 0x84,
    AER_ROOM_CH_BGUNPILLARS = 0x85,
    AER_ROOM_CH_BFINAL = 0x86,
    AER_ROOM_S_GAUNTLETEND = 0x87,
    AER_ROOM_____LEFT = 0x88,
    AER_ROOM_CH_BDIRKDEMOLITION = 0x89,
    AER_ROOM_____LEFT_PATH_1 = 0x8a,
    AER_ROOM_CH_TABIGONE = 0x8b,
    AER_ROOM_CH_CGATEBLOCK = 0x8c,
    AER_ROOM_CH_BMADDASH = 0x8d,
    AER_ROOM_CH_TLONGESTROAD = 0x8e,
    AER_ROOM_S_BULLETBAKER = 0x8f,
    AER_ROOM_CH_CENDHALL = 0x90,
    AER_ROOM_____LEFT_PATH_2 = 0x91,
    AER_ROOM_CH_CTURNHALL = 0x92,
    AER_ROOM_CH_BFPS = 0x93,
    AER_ROOM_CH_CBIGGGNS = 0x94,
    AER_ROOM_CH_CSPAWNGROUND = 0x95,
    AER_ROOM_S_COUNTACULARD = 0x96,
    AER_ROOM_____RIGHT = 0x97,
    AER_ROOM_CH_ACORNER = 0x98,
    AER_ROOM_____RIGHT_PATH_01 = 0x99,
    AER_ROOM_CH_BDIRKDELUGE = 0x9a,
    AER_ROOM_CH_BPODS = 0x9b,
    AER_ROOM_CH_BGUNDIRKDASH = 0x9c,
    AER_ROOM_S_MARKSCYTHE = 0x9d,
    AER_ROOM_S_GAUNTLETLINKUP = 0x9e,
    AER_ROOM_____RIGHT_PATH_02 = 0x9f,
    AER_ROOM_CH_APILLARBIRD = 0xa0,
    AER_ROOM_CH_CSPIRAL = 0xa1,
    AER_ROOM_CH_TBIRDSTANDOFF = 0xa2,
    AER_ROOM_CH_BLEAPERFALL = 0xa3,
    AER_ROOM_S_BENNYARROW = 0xa4,
    AER_ROOM_S_GAUNTLETTITANFINALE = 0xa5,
    AER_ROOM_____OLD_SOUTH = 0xa6,
    AER_ROOM_BOSSSOUTH = 0xa7,
    AER_ROOM_FOLDER_ENDSOUTH = 0xa8,
    AER_ROOM_FOLDER_EAST = 0xa9,
    AER_ROOM______INTRO = 0xaa,
    AER_ROOM_EA_EASTOPENING = 0xab,
    AER_ROOM_EC_SWORDBRIDGE = 0xac,
    AER_ROOM_EL_FLAMEELEVATORENTER = 0xad,
    AER_ROOM_EA_WATERTUNNELLAB = 0xae,
    AER_ROOM_EC_THEPLAZA = 0xaf,
    AER_ROOM_EC_NPCDRUGDEN = 0xb0,
    AER_ROOM_EX_TOWEREAST = 0xb1,
    AER_ROOM_EB_BOGSTREET = 0xb2,
    AER_ROOM_EC_PLAZATOLOOP = 0xb3,
    AER_ROOM______FLAMEDUNGEON = 0xb4,
    AER_ROOM_EL_MEGAHUGELAB = 0xb5,
    AER_ROOM_EB_MELTYMASHARENA = 0xb6,
    AER_ROOM_EB_FLAMEPITLAB = 0xb7,
    AER_ROOM_EL_FLAMEELEVATOREXIT = 0xb8,
    AER_ROOM_EB_DEADOTTERWALK = 0xb9,
    AER_ROOM______SUNKENDOCKS = 0xba,
    AER_ROOM_EC_PLAZAACCESSLAB = 0xbb,
    AER_ROOM_EC_DOCKSLAB = 0xbc,
    AER_ROOM_EX_DOCKSCAMPFIRE = 0xbd,
    AER_ROOM_EV_DOCKSBRIDGE = 0xbe,
    AER_ROOM_EL_FROGARENA = 0xbf,
    AER_ROOM______FROGBOG = 0xc0,
    AER_ROOM_EC_BIGBOGLAB = 0xc1,
    AER_ROOM_EA_BOGTEMPLECAMP = 0xc2,
    AER_ROOM_EA_FROGBOSS = 0xc3,
    AER_ROOM_EC_TEMPLEISHVAULT = 0xc4,
    AER_ROOM______LOOPDELOOP = 0xc5,
    AER_ROOM_EC_EASTLOOP = 0xc6,
    AER_ROOM_EC_LOOPLAB = 0xc7,
    AER_ROOM_EB_MELTYLEAPERARENA = 0xc8,
    AER_ROOM_____OLD_EAST = 0xc9,
    AER_ROOM_EC_PLAZATODOCKS = 0xca,
    AER_ROOM_EA_DOCKFIGHTLAB = 0xcb,
    AER_ROOM_EB_UNDEROTTERBIGRIFLERUMBLE = 0xcc,
    AER_ROOM_EB_CLEANERSHOLE = 0xcd,
    AER_ROOM_FOLDER_ENDEAST = 0xce,
    AER_ROOM_FOLDER_WEST = 0xcf,
    AER_ROOM_____ENTRANCE = 0xd0,
    AER_ROOM_WA_ENTRANCE = 0xd1,
    AER_ROOM_WL_PRISONHALVAULT = 0xd2,
    AER_ROOM_WA_DEADWOOD = 0xd3,
    AER_ROOM_WA_DEADWOODS1 = 0xd4,
    AER_ROOM_WA_GROTTO_BUFFINTRO = 0xd5,
    AER_ROOM_WC_WINDINGWOOD = 0xd6,
    AER_ROOM_WC_GROTTONPC = 0xd7,
    AER_ROOM_WL_NPCTREEHOUSE = 0xd8,
    AER_ROOM_WC_MINILAB = 0xd9,
    AER_ROOM_WT_THEWOOD = 0xda,
    AER_ROOM_WA_ENTSWITCH = 0xdb,
    AER_ROOM_WC_MEADOWOODCORNER = 0xdc,
    AER_ROOM_____TREETRE = 0xdd,
    AER_ROOM_WB_TREETREACHERY = 0xde,
    AER_ROOM_WL_WESTDRIFTERVAULT = 0xdf,
    AER_ROOM_____CRYSTAL_LABS = 0xe0,
    AER_ROOM_WT_SLOWLAB = 0xe1,
    AER_ROOM_WC_CLIFFSIDECELLSREDUX = 0xe2,
    AER_ROOM_WC_PRISONHAL = 0xe3,
    AER_ROOM_____CRYSTAL_LAKE = 0xe4,
    AER_ROOM_WC_THINFOREST = 0xe5,
    AER_ROOM_WC_SIMPLEPATH = 0xe6,
    AER_ROOM_WC_CRYSTALLAKE = 0xe7,
    AER_ROOM_WC_CRYSTALLAKEVAULT = 0xe8,
    AER_ROOM_WC_PRISONHALLEND = 0xe9,
    AER_ROOM_WC_THINFORESTLOW = 0xea,
    AER_ROOM_WC_THINFORESTLOWSECRET = 0xeb,
    AER_ROOM_WA_TITANFALLS = 0xec,
    AER_ROOM_____FOUNDATIONS = 0xed,
    AER_ROOM_WA_VALE = 0xee,
    AER_ROOM_WC_BIGMEADOW = 0xef,
    AER_ROOM_WC_BIGMEADOWVAULT = 0xf0,
    AER_ROOM_WC_MEADOWCAVECROSSING = 0xf1,
    AER_ROOM_WB_BIGBATTLE = 0xf2,
    AER_ROOM_WB_TANUKITROUBLE = 0xf3,
    AER_ROOM_WC_RUINCLEARING = 0xf4,
    AER_ROOM_WX_BOSS = 0xf5,
    AER_ROOM_WA_TOWERENTER = 0xf6,
    AER_ROOM_WA_MULTIENTRANCELAB = 0xf7,
    AER_ROOM_WA_CRSYTALDESCENT = 0xf8,
    AER_ROOM_____OLD_WEST = 0xf9,
    AER_ROOM_WA_GROTTOX = 0xfa,
    AER_ROOM_WB_CRYSTALQUEEN = 0xfb,
    AER_ROOM_WT_PROTOGRID = 0xfc,
    AER_ROOM_WB_PUZZLEPALACE = 0xfd,
    AER_ROOM_FOLDER_ENDWEST = 0xfe,
    AER_ROOM_FOLDER_ABYSS = 0xff,
    AER_ROOM_A_ELEVATORSHAFTUPPER = 0x100,
    AER_ROOM_A_ELEVATORSHAFT = 0x101,
    AER_ROOM_A_PREDOWNWARD = 0x102,
    AER_ROOM_A_DOWNWARD = 0x103,
    AER_ROOM_A_DOWNWARDDEAD = 0x104,
    AER_ROOM_A_DOWNWARDDEADREVISIT = 0x105,
    AER_ROOM_A_EMBERROOM = 0x106,
    AER_ROOM_FOLDER_ENDABYSS = 0x107,
    AER_ROOM_FOLDER_EXTRA = 0x108,
    AER_ROOM_BOSSRUSH_HUB = 0x109,
    AER_ROOM_BOSSRUSH_FROGBOSS = 0x10a,
    AER_ROOM_BOSSRUSH_JERKPOPE = 0x10b,
    AER_ROOM_BOSSRUSH_GENERAL = 0x10c,
    AER_ROOM_BOSSRUSH_BULLETBAKER = 0x10d,
    AER_ROOM_BOSSRUSH_COUNTACULARD = 0x10e,
    AER_ROOM_BOSSRUSH_MARKSCYTHE = 0x10f,
    AER_ROOM_BOSSRUSH_BENNYARROW = 0x110,
    AER_ROOM_BOSSRUSH_EMBER = 0x111,
    AER_ROOM_BOSSRUSH_FINALSCREEN = 0x112,
    AER_ROOM_FOLDER_ENDEXTRA = 0x113
} AERRoomIndex;

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Query the currently active room.
 *
 * @return Index of current room or ::AER_ROOM_NULL if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 *
 * @since 1.0.0
 */
int32_t AERRoomGetCurrent(void);

/**
 * @brief Change the currently active room to a new one.
 *
 * This is a low-level engine function that will not create an instance of
 * ::AER_OBJECT_CHAR in the target room. ::AERRoomEnter is preferrable in the
 * majority of usecases.
 *
 * @param[in] roomIdx Index of new room.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage or if a room change is
 * already in progress.
 * @throw ::AER_FAILED_LOOKUP if argument `roomIdx` is an invalid room.
 *
 * @since 1.0.0
 *
 * @sa AERRoomEnter
 * @sa AERRoomEnterWithPosition
 */
void AERRoomGoto(int32_t roomIdx);

/**
 * @brief Change the currently active room to a new one and prepare the room
 * context.
 *
 * If argument `roomIdx` is a "menu" room, then this function behaves similarly
 * to ::AERRoomGoto. However, if `roomIdx` is an in-game level, then this
 * function creates an instance of ::AER_OBJECT_CHAR in the room's default
 * position.
 *
 * @param[in] roomIdx Index of new room.
 * @param[in] fade Whether or not to smoothly transition to new room.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage or if a room change is
 * already in progress.
 * @throw ::AER_FAILED_LOOKUP if argument `roomIdx` is an invalid room.
 *
 * @since 1.5.0
 *
 * @sa AERRoomEnterWithPosition
 * @sa AERRoomGoto
 */
void AERRoomEnter(int32_t roomIdx, bool fade);

/**
 * @brief Change the currently active room to a new one and prepare the room
 * context.
 *
 * This function behaves the same as ::AERRoomEnter except that if an instance
 * of ::AER_OBJECT_CHAR is created, then it is created at the provided location
 * in the new room rather than the default.
 *
 * @param[in] roomIdx Index of new room.
 * @param[in] fade Whether or not to smoothly transition to new room.
 * @param[in] x Horizontal position in new room at which to create player
 * instance.
 * @param[in] y Vertical position in new room at which to create player
 * instance.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage or if a room change is
 * already in progress.
 * @throw ::AER_FAILED_LOOKUP if argument `roomIdx` is an invalid room.
 *
 * @since 1.5.0
 *
 * @sa AERRoomEnter
 * @sa AERRoomGoto
 */
void AERRoomEnterWithPosition(int32_t roomIdx, bool fade, float x, float y);

/**
 * @brief Query the room with a specific name.
 *
 * @param[in] name Name of room.
 *
 * @return Room index or ::AER_ROOM_NULL if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_NULL_ARG if argument `name` is `NULL`.
 * @throw ::AER_FAILED_LOOKUP if no room with name given by argument `name`.
 *
 * @since 1.3.0
 */
int32_t AERRoomGetByName(const char* name);

/**
 * @brief Query the name of a room.
 *
 * @param[in] roomIdx Room of interest.
 *
 * @return Name of room or `NULL` if unsuccessful.
 *
 * @throw ::AER_SEQ_BREAK if called outside action stage.
 * @throw ::AER_FAILED_LOOKUP if argument `roomIdx` is an invalid sprite.
 *
 * @since 1.3.0
 */
const char* AERRoomGetName(int32_t roomIdx);

#endif /* AER_ROOM_H */
