/**
 * @file
 *
 * @brief Utilities for querying and manipulating audio samples and playback.
 *
 * @since 1.3.0
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
#ifndef AER_AUDIO_H
#define AER_AUDIO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Vanilla audio samples.
 *
 * @since 1.4.0
 */
typedef enum AERAudioSampleIndex {
    /**
     * @brief Flag which represents either no sample or an invalid sample
     * depending on context.
     */
    AER_SAMPLE_NULL = -1,
    AER_SAMPLE_SILENCE = 0x0,
    AER_SAMPLE_NULLKEEPATTOP = 0x1,
    AER_SAMPLE_BOSSDEFEATEDTONE = 0x2,
    AER_SAMPLE_BIRDBOSSHIT1 = 0x3,
    AER_SAMPLE_BIRDBOSSHIT2 = 0x4,
    AER_SAMPLE_BIRDBOSSHIT3 = 0x5,
    AER_SAMPLE_FROGBOSSHIT1 = 0x6,
    AER_SAMPLE_FROGBOSSHIT2 = 0x7,
    AER_SAMPLE_FROGBOSSHIT3 = 0x8,
    AER_SAMPLE_FROGBOSSPUKE = 0x9,
    AER_SAMPLE_FROGBOSSDEATH = 0xa,
    AER_SAMPLE_FROGBOSSINTRO = 0xb,
    AER_SAMPLE_TANUKIBOSSHIT1 = 0xc,
    AER_SAMPLE_TANUKIBOSSHIT2 = 0xd,
    AER_SAMPLE_TANUKIBOSSHIT3 = 0xe,
    AER_SAMPLE_ALUCARDINTRO = 0xf,
    AER_SAMPLE_ALUCARDDEATH = 0x10,
    AER_SAMPLE_ALUCARDSHOOT = 0x11,
    AER_SAMPLE_ALUCARDDASH = 0x12,
    AER_SAMPLE_ALUCARDDRONEDEATH = 0x13,
    AER_SAMPLE_BULLETBAKERINTRO = 0x14,
    AER_SAMPLE_BULLETBAKERDEATH = 0x15,
    AER_SAMPLE_BULLETBAKERSUMMON = 0x16,
    AER_SAMPLE_BULLETBAKERFIRE = 0x17,
    AER_SAMPLE_MARKSCYTHEINTRO = 0x18,
    AER_SAMPLE_MARKSCYTHERUN = 0x19,
    AER_SAMPLE_MARKSCYTHEDEATH = 0x1a,
    AER_SAMPLE_MARKSCYTHECATCHSCYTHE = 0x1b,
    AER_SAMPLE_MARKSCYTHESLAM = 0x1c,
    AER_SAMPLE_SCYTHETHROW = 0x1d,
    AER_SAMPLE_SCYTHESPIN = 0x1e,
    AER_SAMPLE_BENNYARROWINTRO = 0x1f,
    AER_SAMPLE_BENNYARROWDEATH = 0x20,
    AER_SAMPLE_ARROWLAND = 0x21,
    AER_SAMPLE_ARROWFIRE = 0x22,
    AER_SAMPLE_HALDIAMONDSUMMON = 0x23,
    AER_SAMPLE_HALLOOP = 0x24,
    AER_SAMPLE_HALHURT = 0x25,
    AER_SAMPLE_HALSCREAM = 0x26,
    AER_SAMPLE_HALWARPIN = 0x27,
    AER_SAMPLE_HALBULLET = 0x28,
    AER_SAMPLE_HALDASH = 0x29,
    AER_SAMPLE_HALFIREBALL = 0x2a,
    AER_SAMPLE_HALARMREELBACK = 0x2b,
    AER_SAMPLE_HALARMLUNGE = 0x2c,
    AER_SAMPLE_HALFLASH = 0x2d,
    AER_SAMPLE_HALFALL = 0x2e,
    AER_SAMPLE_HALDEAD = 0x2f,
    AER_SAMPLE_HALDASHATTACKCHARGE = 0x30,
    AER_SAMPLE_HALDASHATTACK = 0x31,
    AER_SAMPLE_HALPURPLEBEAM = 0x32,
    AER_SAMPLE_TANUKIGENERALSHOOT = 0x33,
    AER_SAMPLE_TANUKIGENERALROAR = 0x34,
    AER_SAMPLE_TANUKIGENERALSWORDSWING = 0x35,
    AER_SAMPLE_TANUKIGENERALDIE = 0x36,
    AER_SAMPLE_FROGBOSSSMASH = 0x37,
    AER_SAMPLE_FROGBOSSBOMBEXPLODE = 0x38,
    AER_SAMPLE_FROGBOSSBOMB = 0x39,
    AER_SAMPLE_FROGBOSSROAR = 0x3a,
    AER_SAMPLE_FROGBOSSJUMP = 0x3b,
    AER_SAMPLE_JERKPOPEINTRO = 0x3c,
    AER_SAMPLE_JERKPOPESUMMON = 0x3d,
    AER_SAMPLE_JERKPOPEDEATH = 0x3e,
    AER_SAMPLE_JERKPOPETELEPORT = 0x3f,
    AER_SAMPLE_JERKPOPESLAMSTAFF = 0x40,
    AER_SAMPLE_MUSABOSS1 = 0x41,
    AER_SAMPLE_MUSABOSS2 = 0x42,
    AER_SAMPLE_MUSABOSS3 = 0x43,
    AER_SAMPLE_MUSABOSSDEAD = 0x44,
    AER_SAMPLE_HALDOORMUSIC = 0x45,
    AER_SAMPLE_MUSABOSSP = 0x46,
    AER_SAMPLE_MUSADOWNWARD = 0x47,
    AER_SAMPLE_MUSAELEV0 = 0x48,
    AER_SAMPLE_MUSAELEV1 = 0x49,
    AER_SAMPLE_MUSSSRFC = 0x4a,
    AER_SAMPLE_MUSAMBHATCHERY = 0x4b,
    AER_SAMPLE_MUSAMBLABBELL = 0x4c,
    AER_SAMPLE_MUSAMBMACHINE1 = 0x4d,
    AER_SAMPLE_MUSAMBMACHINE2 = 0x4e,
    AER_SAMPLE_MUSAMBMACHINE3 = 0x4f,
    AER_SAMPLE_MUSAMBMACHINE4 = 0x50,
    AER_SAMPLE_MUSAMBRAINHEAVY2 = 0x51,
    AER_SAMPLE_MUSAMBTABLET = 0x52,
    AER_SAMPLE_MUSAELEV2 = 0x53,
    AER_SAMPLE_MUSAELEV3 = 0x54,
    AER_SAMPLE_MUSAMBLABWATERDROPS3 = 0x55,
    AER_SAMPLE_MUSAMBHULLOUTSIDE = 0x56,
    AER_SAMPLE_MUSAMBHULLINSIDE = 0x57,
    AER_SAMPLE_MUSAMBEYEBALLS = 0x58,
    AER_SAMPLE_MUSAMBWINDBUILDINGS1 = 0x59,
    AER_SAMPLE_MUSCDP = 0x5a,
    AER_SAMPLE_MUSAELEV4 = 0x5b,
    AER_SAMPLE_MUSAELEV5 = 0x5c,
    AER_SAMPLE_MUSAELEV6 = 0x5d,
    AER_SAMPLE_MUSAPREBOSS = 0x5e,
    AER_SAMPLE_MUSAMBAQUARIUM1 = 0x5f,
    AER_SAMPLE_MUSAMBAQUARIUM2 = 0x60,
    AER_SAMPLE_MUSAMBCMARKET1 = 0x61,
    AER_SAMPLE_MUSAMBCAMPFIRE = 0x62,
    AER_SAMPLE_MUSAMBCURTAIN1 = 0x63,
    AER_SAMPLE_MUSAMBCURTAIN2 = 0x64,
    AER_SAMPLE_MUSAMBDEADROBOT1 = 0x65,
    AER_SAMPLE_MUSAMBEGAZEBO = 0x66,
    AER_SAMPLE_MUSAMBFLAG1 = 0x67,
    AER_SAMPLE_MUSAMBFLAG2 = 0x68,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE1 = 0x69,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE2 = 0x6a,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE3 = 0x6b,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE4 = 0x6c,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE5 = 0x6d,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE6 = 0x6e,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE7 = 0x6f,
    AER_SAMPLE_MUSAMBFLAMEBOTTLE8 = 0x70,
    AER_SAMPLE_MUSAMBGARDEN1 = 0x71,
    AER_SAMPLE_MUSAMBHISS = 0x72,
    AER_SAMPLE_MUSAMBHYPERLIGHT01L = 0x73,
    AER_SAMPLE_MUSAMBHYPERLIGHT01R = 0x74,
    AER_SAMPLE_MUSAMBHYPERLIGHT0 = 0x75,
    AER_SAMPLE_MUSAMBHYPERLIGHT1 = 0x76,
    AER_SAMPLE_MUSAMBHYPERLIGHT2 = 0x77,
    AER_SAMPLE_MUSAMBINSTORMAMBIENCE = 0x78,
    AER_SAMPLE_MUSAMBLABAMBIENCE1 = 0x79,
    AER_SAMPLE_MUSAMBLABAMBIENCE2 = 0x7a,
    AER_SAMPLE_MUSAMBLABAMBIENCE3 = 0x7b,
    AER_SAMPLE_MUSAMBLABHUM1 = 0x7c,
    AER_SAMPLE_MUSAMBLABHUM2 = 0x7d,
    AER_SAMPLE_MUSAMBLABWATERDROPS1 = 0x7e,
    AER_SAMPLE_MUSAMBLABWATERDROPS2 = 0x7f,
    AER_SAMPLE_MUSAMBMONOMACHINE1 = 0x80,
    AER_SAMPLE_MUSAMBMONOMACHINE2 = 0x81,
    AER_SAMPLE_MUSAMBMONOSIZZLE = 0x82,
    AER_SAMPLE_MUSAMBMONOTERMINAL1 = 0x83,
    AER_SAMPLE_MUSAMBMONOTERMINAL2 = 0x84,
    AER_SAMPLE_MUSAMBMONOTERMINAL3 = 0x85,
    AER_SAMPLE_MUSAMBMONOTRANSFORMER1 = 0x86,
    AER_SAMPLE_MUSAMBMONOTREERUSTLE1 = 0x87,
    AER_SAMPLE_MUSAMBNSNOW2 = 0x88,
    AER_SAMPLE_MUSAMBRAIN = 0x89,
    AER_SAMPLE_MUSAMBRAINHEAVY1 = 0x8a,
    AER_SAMPLE_MUSAMBRAINLIGHT1 = 0x8b,
    AER_SAMPLE_MUSAMBRAINLIGHT2 = 0x8c,
    AER_SAMPLE_MUSAMBRUMBLE = 0x8d,
    AER_SAMPLE_MUSAMBSEWERGRATE1 = 0x8e,
    AER_SAMPLE_MUSAMBSWAMP1 = 0x8f,
    AER_SAMPLE_MUSAMBSWARM = 0x90,
    AER_SAMPLE_MUSAMBTORCHLAMP1 = 0x91,
    AER_SAMPLE_MUSAMBTORCHLAMP2 = 0x92,
    AER_SAMPLE_MUSAMBTORCHLAMP3 = 0x93,
    AER_SAMPLE_MUSAMBTORCHLAMP4 = 0x94,
    AER_SAMPLE_MUSAMBTORCHLAMP5 = 0x95,
    AER_SAMPLE_MUSAMBTORCHOPEN1 = 0x96,
    AER_SAMPLE_MUSAMBTREECREAK1 = 0x97,
    AER_SAMPLE_MUSAMBTREERUSTLE1 = 0x98,
    AER_SAMPLE_MUSAMBTREERUSTLE2 = 0x99,
    AER_SAMPLE_MUSAMBTREESWAMP1 = 0x9a,
    AER_SAMPLE_MUSAMBVAT0 = 0x9b,
    AER_SAMPLE_MUSAMBVAT1 = 0x9c,
    AER_SAMPLE_MUSAMBVAT2 = 0x9d,
    AER_SAMPLE_MUSAMBVAT3 = 0x9e,
    AER_SAMPLE_MUSAMBVAULT = 0x9f,
    AER_SAMPLE_MUSAMBVOIDLARGE1 = 0xa0,
    AER_SAMPLE_MUSAMBVOIDMEDIUM1 = 0xa1,
    AER_SAMPLE_MUSAMBVOIDSMALL1 = 0xa2,
    AER_SAMPLE_MUSAMBVOIDWATER1 = 0xa3,
    AER_SAMPLE_MUSAMBWATERDARK1 = 0xa4,
    AER_SAMPLE_MUSAMBWATERFOUNTAINHEADLARGE1 = 0xa5,
    AER_SAMPLE_MUSAMBWATERFOUNTAINHEADLARGE2 = 0xa6,
    AER_SAMPLE_MUSAMBWATERFOUNTAINHEADSMALL1 = 0xa7,
    AER_SAMPLE_MUSAMBWATERFOUNTAINHEADSMALL2 = 0xa8,
    AER_SAMPLE_MUSAMBWATERFOUNTAINS1 = 0xa9,
    AER_SAMPLE_MUSAMBWATERLAB0 = 0xaa,
    AER_SAMPLE_MUSAMBWATERLAB1 = 0xab,
    AER_SAMPLE_MUSAMBWATERSRFC0 = 0xac,
    AER_SAMPLE_MUSAMBWATERTITANHEAD = 0xad,
    AER_SAMPLE_MUSAMBWATERFALLL1 = 0xae,
    AER_SAMPLE_MUSAMBWATERFALLLAB1 = 0xaf,
    AER_SAMPLE_MUSAMBWATERFALLM1 = 0xb0,
    AER_SAMPLE_MUSAMBWATERFALLS1 = 0xb1,
    AER_SAMPLE_MUSAMBWEIRD0 = 0xb2,
    AER_SAMPLE_MUSAMBWESTWIND0 = 0xb3,
    AER_SAMPLE_MUSAMBWESTWIND1 = 0xb4,
    AER_SAMPLE_MUSAMBWINDCOLD1 = 0xb5,
    AER_SAMPLE_MUSAMBWINDCOLD2 = 0xb6,
    AER_SAMPLE_MUSAMBWINDCOLD3 = 0xb7,
    AER_SAMPLE_MUSAMBWINDCOLD4 = 0xb8,
    AER_SAMPLE_MUSAMBWINDCOLD5 = 0xb9,
    AER_SAMPLE_MUSAMBWINDCOLD6 = 0xba,
    AER_SAMPLE_MUSAMBWINDCRYSTAL1 = 0xbb,
    AER_SAMPLE_MUSAMBWINDEERIE0 = 0xbc,
    AER_SAMPLE_MUSAMBWINDNARROW1 = 0xbd,
    AER_SAMPLE_MUSAMBWINDNEUTRAL0 = 0xbe,
    AER_SAMPLE_MUSAMBWINDNEUTRAL1 = 0xbf,
    AER_SAMPLE_MUSB0 = 0xc0,
    AER_SAMPLE_MUSB1 = 0xc1,
    AER_SAMPLE_MUSBR0 = 0xc2,
    AER_SAMPLE_MUSBR1 = 0xc3,
    AER_SAMPLE_MUSBR2 = 0xc4,
    AER_SAMPLE_MUSBR3 = 0xc5,
    AER_SAMPLE_MUSBR4 = 0xc6,
    AER_SAMPLE_MUSBR5 = 0xc7,
    AER_SAMPLE_MUSCABYSSDOOR = 0xc8,
    AER_SAMPLE_MUSCAPT = 0xc9,
    AER_SAMPLE_MUSCDREGS = 0xca,
    AER_SAMPLE_MUSCTOWN = 0xcb,
    AER_SAMPLE_MUSCREDITS = 0xcc,
    AER_SAMPLE_MUSDRONE1 = 0xcd,
    AER_SAMPLE_MUSEFBBOSS = 0xce,
    AER_SAMPLE_MUSEFBLAB1 = 0xcf,
    AER_SAMPLE_MUSEFBLAB2 = 0xd0,
    AER_SAMPLE_MUSEFBSRFC1 = 0xd1,
    AER_SAMPLE_MUSEFBSRFC2 = 0xd2,
    AER_SAMPLE_MUSEFBSRFC3 = 0xd3,
    AER_SAMPLE_MUSEFD0 = 0xd4,
    AER_SAMPLE_MUSEFD1 = 0xd5,
    AER_SAMPLE_MUSEFD2 = 0xd6,
    AER_SAMPLE_MUSEFD3 = 0xd7,
    AER_SAMPLE_MUSEFD4 = 0xd8,
    AER_SAMPLE_MUSNCD1 = 0xd9,
    AER_SAMPLE_MUSNCD2 = 0xda,
    AER_SAMPLE_MUSNCD3 = 0xdb,
    AER_SAMPLE_MUSEFD5 = 0xdc,
    AER_SAMPLE_MUSEFD6 = 0xdd,
    AER_SAMPLE_MUSELLLABA = 0xde,
    AER_SAMPLE_MUSELLLABC = 0xdf,
    AER_SAMPLE_MUSELLSRFCC = 0xe0,
    AER_SAMPLE_MUSESDLABA = 0xe1,
    AER_SAMPLE_MUSESDLABAA = 0xe2,
    AER_SAMPLE_MUSESDLABC = 0xe3,
    AER_SAMPLE_MUSESDSRFCA = 0xe4,
    AER_SAMPLE_MUSESDSRFCC = 0xe5,
    AER_SAMPLE_MUSESDSRFCT = 0xe6,
    AER_SAMPLE_MUSEILAB = 0xe7,
    AER_SAMPLE_MUSEIA = 0xe8,
    AER_SAMPLE_MUSEISRFCA = 0xe9,
    AER_SAMPLE_MUSEISRFCB = 0xea,
    AER_SAMPLE_MUSEICHURCH = 0xeb,
    AER_SAMPLE_MUSEISWORD = 0xec,
    AER_SAMPLE_MUSEAST01 = 0xed,
    AER_SAMPLE_MUSEAST01TITAN = 0xee,
    AER_SAMPLE_MUSGAUNTLET00 = 0xef,
    AER_SAMPLE_MUSGAUNTLET01 = 0xf0,
    AER_SAMPLE_MUSGAUNTLET02 = 0xf1,
    AER_SAMPLE_MUSGAUNTLET03 = 0xf2,
    AER_SAMPLE_MUSGAUNTLET04A1 = 0xf3,
    AER_SAMPLE_MUSGAUNTLET04A2 = 0xf4,
    AER_SAMPLE_MUSGAUNTLET04B1 = 0xf5,
    AER_SAMPLE_MUSGAUNTLET04B2 = 0xf6,
    AER_SAMPLE_MUSGAUNTLET05 = 0xf7,
    AER_SAMPLE_MUSGAUNTLET06 = 0xf8,
    AER_SAMPLE_MUSGAUNTLET07 = 0xf9,
    AER_SAMPLE_MUSGAUNTLET08 = 0xfa,
    AER_SAMPLE_MUSGAUNTLET09 = 0xfb,
    AER_SAMPLE_MUSGAUNTLET10 = 0xfc,
    AER_SAMPLE_MUSGAUNTLET11 = 0xfd,
    AER_SAMPLE_MUSGAUNTLET12 = 0xfe,
    AER_SAMPLE_MUSGAUNTLET13A = 0xff,
    AER_SAMPLE_MUSGAUNTLET13B = 0x100,
    AER_SAMPLE_MUSGAUNTLET14 = 0x101,
    AER_SAMPLE_MUSGAUNTLET15 = 0x102,
    AER_SAMPLE_MUSILAB0 = 0x103,
    AER_SAMPLE_MUSITITLE = 0x104,
    AER_SAMPLE_MUSITONE = 0x105,
    AER_SAMPLE_MUSNCATHEDRAL0 = 0x106,
    AER_SAMPLE_MUSNCATHEDRAL1 = 0x107,
    AER_SAMPLE_MUSNCATHEDRAL2 = 0x108,
    AER_SAMPLE_MUSNCATHEDRALX = 0x109,
    AER_SAMPLE_MUSNGAP0 = 0x10a,
    AER_SAMPLE_MUSNGAP1 = 0x10b,
    AER_SAMPLE_MUSNGAPX = 0x10c,
    AER_SAMPLE_MUSNWRLAB1 = 0x10d,
    AER_SAMPLE_MUSNWRSRFC1 = 0x10e,
    AER_SAMPLE_MUSNWRSRFC2 = 0x10f,
    AER_SAMPLE_MUSNWRSRFC3 = 0x110,
    AER_SAMPLE_MUSNWRX = 0x111,
    AER_SAMPLE_MUSTOWER = 0x112,
    AER_SAMPLE_MUSWBOSS = 0x113,
    AER_SAMPLE_MUSWCLPHA = 0x114,
    AER_SAMPLE_MUSWCLSLA = 0x115,
    AER_SAMPLE_MUSWCLSLC = 0x116,
    AER_SAMPLE_MUSWE2A = 0x117,
    AER_SAMPLE_MUSWLKSRFC1 = 0x118,
    AER_SAMPLE_MUSWLKSRFC2 = 0x119,
    AER_SAMPLE_MUSWLKSRFC3 = 0x11a,
    AER_SAMPLE_MUSWVBATTLE1 = 0x11b,
    AER_SAMPLE_MUSWVBATTLE2 = 0x11c,
    AER_SAMPLE_MUSWVBATTLE3 = 0x11d,
    AER_SAMPLE_MUSWVBATTLE4 = 0x11e,
    AER_SAMPLE_MUSWE1 = 0x11f,
    AER_SAMPLE_MUSWE0 = 0x120,
    AER_SAMPLE_MUSWE2B = 0x121,
    AER_SAMPLE_MUSWE2C = 0x122,
    AER_SAMPLE_MUSWE2BLAB = 0x123,
    AER_SAMPLE_MUSWE3 = 0x124,
    AER_SAMPLE_MUSBADASSDEATH = 0x125,
    AER_SAMPLE_BADASSJINGLE = 0x126,
    AER_SAMPLE_MXBADASSDEATH1 = 0x127,
    AER_SAMPLE_MXBADASSDEATH2 = 0x128,
    AER_SAMPLE_MXINTROCLIFF = 0x129,
    AER_SAMPLE_HALMUSIC = 0x12a,
    AER_SAMPLE_EARTHQUAKE = 0x12b,
    AER_SAMPLE_ENTEREDNEWREGION = 0x12c,
    AER_SAMPLE_AMBRAIN = 0x12d,
    AER_SAMPLE_AMBABYSS = 0x12e,
    AER_SAMPLE_AMBABYSSFINALBOSS = 0x12f,
    AER_SAMPLE_AMBNORTH = 0x130,
    AER_SAMPLE_AMBWRBIRDWIND = 0x131,
    AER_SAMPLE_AMBWRGRASSWIND = 0x132,
    AER_SAMPLE_AMBWRLIGHTWIND = 0x133,
    AER_SAMPLE_AMBSNOW = 0x134,
    AER_SAMPLE_AMBNORTH2 = 0x135,
    AER_SAMPLE_AMBNORTH3 = 0x136,
    AER_SAMPLE_MUSAMBBIRDSCHANTING = 0x137,
    AER_SAMPLE_DIRKFALL = 0x138,
    AER_SAMPLE_FROGNINJASTARTHROW = 0x139,
    AER_SAMPLE_SHRUBWIGGLE = 0x13a,
    AER_SAMPLE_SPIRALBOMBTHROW = 0x13b,
    AER_SAMPLE_PLANTYATTACK = 0x13c,
    AER_SAMPLE_PLANTYDEATH = 0x13d,
    AER_SAMPLE_SPIRALBOMBSHOOT = 0x13e,
    AER_SAMPLE_TANUKIGUNAPPEAR = 0x13f,
    AER_SAMPLE_BIRDATTACKSCREECH = 0x140,
    AER_SAMPLE_BIRDDEATH = 0x141,
    AER_SAMPLE_BIRDTELEPORT = 0x142,
    AER_SAMPLE_BISHOPBIRDLASER = 0x143,
    AER_SAMPLE_BLADEBOTSPAWN = 0x144,
    AER_SAMPLE_CRYSTALCATCH = 0x145,
    AER_SAMPLE_CRYSTALENEMYHIT1 = 0x146,
    AER_SAMPLE_CRYSTALENEMYHIT2 = 0x147,
    AER_SAMPLE_CRYSTALGOLEMDEATH = 0x148,
    AER_SAMPLE_CRYSTALSPIDERATTACK = 0x149,
    AER_SAMPLE_CRYSTALSPIDERDEATH = 0x14a,
    AER_SAMPLE_CRYSTALSPIDERWALK = 0x14b,
    AER_SAMPLE_CULTBIRDSPEAR = 0x14c,
    AER_SAMPLE_DIRKATTACK = 0x14d,
    AER_SAMPLE_DIRKCLAW = 0x14e,
    AER_SAMPLE_DIRKCOMMANDERFIRE = 0x14f,
    AER_SAMPLE_DIRKDEATH = 0x150,
    AER_SAMPLE_DIRKOMMANDERROAR = 0x151,
    AER_SAMPLE_DIRKROAR = 0x152,
    AER_SAMPLE_DIVEBIRDATTACK = 0x153,
    AER_SAMPLE_DIVEBOMBERATTACK = 0x154,
    AER_SAMPLE_DODGE = 0x155,
    AER_SAMPLE_ENEMYDEATH1V2 = 0x156,
    AER_SAMPLE_ENEMYDEATH2V2 = 0x157,
    AER_SAMPLE_ENEMYDEATH3V2 = 0x158,
    AER_SAMPLE_ENEMYFALLSPLAT = 0x159,
    AER_SAMPLE_FIREFLYBUZZ = 0x15a,
    AER_SAMPLE_FLICKER = 0x15b,
    AER_SAMPLE_FROGDEATH = 0x15c,
    AER_SAMPLE_GOLEMFOOTSTEP1 = 0x15d,
    AER_SAMPLE_GOLEMFOOTSTEP2 = 0x15e,
    AER_SAMPLE_HYPERLIGHTWEAPONSLASH = 0x15f,
    AER_SAMPLE_LEAPERJUMP = 0x160,
    AER_SAMPLE_LEAPERLAND = 0x161,
    AER_SAMPLE_LEAPERLANDNOROAR = 0x162,
    AER_SAMPLE_MISSILELAUNCHER = 0x163,
    AER_SAMPLE_MISSILELOOP = 0x164,
    AER_SAMPLE_PLANTYGROWL = 0x165,
    AER_SAMPLE_RIFLEDIRKSHOT = 0x166,
    AER_SAMPLE_ROBOTDEATH = 0x167,
    AER_SAMPLE_ROBOTHIT1 = 0x168,
    AER_SAMPLE_ROBOTHIT2 = 0x169,
    AER_SAMPLE_ROBOTHIT3 = 0x16a,
    AER_SAMPLE_ROBOTEXPLODE = 0x16b,
    AER_SAMPLE_SHRUBGURGLE = 0x16c,
    AER_SAMPLE_SPIDERATTACK = 0x16d,
    AER_SAMPLE_SPIDERDEATH = 0x16e,
    AER_SAMPLE_SPIDERDROPS = 0x16f,
    AER_SAMPLE_TANUKICRYSTALBREAK = 0x170,
    AER_SAMPLE_TANUKIDEATH = 0x171,
    AER_SAMPLE_TANUKISHOOT = 0x172,
    AER_SAMPLE_TANUKISHOOTHITWALL = 0x173,
    AER_SAMPLE_TANUKISHOOTTRAVEL = 0x174,
    AER_SAMPLE_TURRETACTIVATE = 0x175,
    AER_SAMPLE_TURRETLASERFIRE = 0x176,
    AER_SAMPLE_TURRETORBFIRE = 0x177,
    AER_SAMPLE_WARPBLOCK = 0x178,
    AER_SAMPLE_WOLFATTACK = 0x179,
    AER_SAMPLE_WOLFHOWL = 0x17a,
    AER_SAMPLE_WOLFMOVE = 0x17b,
    AER_SAMPLE_LIZARDEAT = 0x17c,
    AER_SAMPLE_LIZARDSHIDE = 0x17d,
    AER_SAMPLE_BUFFALO1MUMBLE = 0x17e,
    AER_SAMPLE_BUFFALO2EXHALE = 0x17f,
    AER_SAMPLE_BUFFALO3GRUFF = 0x180,
    AER_SAMPLE_BUFFALO4SHRUG = 0x181,
    AER_SAMPLE_BUFFALOSMOKE1 = 0x182,
    AER_SAMPLE_BUFFALOSMOKE2 = 0x183,
    AER_SAMPLE_BUFFALOTRITONE = 0x184,
    AER_SAMPLE_DOGBARK1 = 0x185,
    AER_SAMPLE_DOGBARK2 = 0x186,
    AER_SAMPLE_NPCBADASS1 = 0x187,
    AER_SAMPLE_NPCBADASS2 = 0x188,
    AER_SAMPLE_NPCBADASSDIE = 0x189,
    AER_SAMPLE_NPCBIRDFEMALE1 = 0x18a,
    AER_SAMPLE_NPCBIRDFEMALE2 = 0x18b,
    AER_SAMPLE_NPCBIRDMALE1 = 0x18c,
    AER_SAMPLE_NPCBIRDMALE2 = 0x18d,
    AER_SAMPLE_NPCBOT1 = 0x18e,
    AER_SAMPLE_NPCBOT2 = 0x18f,
    AER_SAMPLE_NPCFROGFEMALE1 = 0x190,
    AER_SAMPLE_NPCFROGFEMALE2 = 0x191,
    AER_SAMPLE_NPCFROGMALE1 = 0x192,
    AER_SAMPLE_NPCFROGMALE2 = 0x193,
    AER_SAMPLE_NPCGENERICFEMALE1 = 0x194,
    AER_SAMPLE_NPCGENERICFEMALE2 = 0x195,
    AER_SAMPLE_NPCGENERICMALE1 = 0x196,
    AER_SAMPLE_NPCGENERICMALE2 = 0x197,
    AER_SAMPLE_NPCLIZARDFEMALE1 = 0x198,
    AER_SAMPLE_NPCLIZARDFEMALE2 = 0x199,
    AER_SAMPLE_NPCLIZARDMALE1 = 0x19a,
    AER_SAMPLE_NPCLIZARDMALE2 = 0x19b,
    AER_SAMPLE_NPCOTTERFEMALE1 = 0x19c,
    AER_SAMPLE_NPCOTTERFEMALE2 = 0x19d,
    AER_SAMPLE_NPCOTTERMALE1 = 0x19e,
    AER_SAMPLE_NPCOTTERMALE2 = 0x19f,
    AER_SAMPLE_NPCTANOOKIFEMALE1 = 0x1a0,
    AER_SAMPLE_NPCTANOOKIFEMALE2 = 0x1a1,
    AER_SAMPLE_NPCTANOOKIMALE1 = 0x1a2,
    AER_SAMPLE_NPCTANOOKIMALE2 = 0x1a3,
    AER_SAMPLE_SOUTHDRONESCAN = 0x1a4,
    AER_SAMPLE_SOUTHDRONEBEEPLOOP = 0x1a5,
    AER_SAMPLE_WEAPONCOLLECT = 0x1a6,
    AER_SAMPLE_AUTOSAVE = 0x1a7,
    AER_SAMPLE_COMPANIONERROR = 0x1a8,
    AER_SAMPLE_COMPANIONPING = 0x1a9,
    AER_SAMPLE_DEATH = 0x1aa,
    AER_SAMPLE_GENERALINTERACT = 0x1ab,
    AER_SAMPLE_GENERICERROR = 0x1ac,
    AER_SAMPLE_INTERACTDOOR = 0x1ad,
    AER_SAMPLE_INTERACTPLATFORM = 0x1ae,
    AER_SAMPLE_INTERACTTERMINAL = 0x1af,
    AER_SAMPLE_LEVELUP = 0x1b0,
    AER_SAMPLE_MENULOADCOMPLETE = 0x1b1,
    AER_SAMPLE_XP = 0x1b2,
    AER_SAMPLE_MAPMODULEAPPEAR = 0x1b3,
    AER_SAMPLE_MENUACTION = 0x1b4,
    AER_SAMPLE_MENUCASCADE = 0x1b5,
    AER_SAMPLE_MENUCLOSE = 0x1b6,
    AER_SAMPLE_MENULOADING = 0x1b7,
    AER_SAMPLE_MENUMOVE = 0x1b8,
    AER_SAMPLE_MENUOPEN = 0x1b9,
    AER_SAMPLE_MENUOPENALT = 0x1ba,
    AER_SAMPLE_MENUSCROLL = 0x1bb,
    AER_SAMPLE_MODULEWINDOWPIP = 0x1bc,
    AER_SAMPLE_STORYBUBBLE = 0x1bd,
    AER_SAMPLE_TITLESCREENSTART = 0x1be,
    AER_SAMPLE_VENDORPURCHASE = 0x1bf,
    AER_SAMPLE_ALTSWORD1 = 0x1c0,
    AER_SAMPLE_ALTSWORD2 = 0x1c1,
    AER_SAMPLE_SSWORD1 = 0x1c2,
    AER_SAMPLE_SSWORD2 = 0x1c3,
    AER_SAMPLE_SWORDLIGHTV4 = 0x1c4,
    AER_SAMPLE_SWORDMEDIUMV4 = 0x1c5,
    AER_SAMPLE_SWORDHEAVYV5 = 0x1c6,
    AER_SAMPLE_SWORDHITENEMY1V2 = 0x1c7,
    AER_SAMPLE_SWORDHITENEMY2V2 = 0x1c8,
    AER_SAMPLE_SWORDHITWALL = 0x1c9,
    AER_SAMPLE_SWORDCHARGE = 0x1ca,
    AER_SAMPLE_SWORDCOMBO = 0x1cb,
    AER_SAMPLE_SWORDCOMBOMISS = 0x1cc,
    AER_SAMPLE_RISINGPLATFORMDEACTIVATE = 0x1cd,
    AER_SAMPLE_CRITSTRIKE = 0x1ce,
    AER_SAMPLE_CRITSTRIKEOLD = 0x1cf,
    AER_SAMPLE_CUT = 0x1d0,
    AER_SAMPLE_PARRY = 0x1d1,
    AER_SAMPLE_BLADEPIERCESFLESH1 = 0x1d2,
    AER_SAMPLE_BLADEPIERCESFLESH2 = 0x1d3,
    AER_SAMPLE_BLADEPIERCESFLESH3 = 0x1d4,
    AER_SAMPLE_REFLECTHEAVY = 0x1d5,
    AER_SAMPLE_REFLECTLIGHT = 0x1d6,
    AER_SAMPLE_DEFLECT = 0x1d7,
    AER_SAMPLE_MISSILECUTV2 = 0x1d8,
    AER_SAMPLE_ROLLYPOLYEXPLODE = 0x1d9,
    AER_SAMPLE_ROLLYPOLLYTHROW1 = 0x1da,
    AER_SAMPLE_ROLLYPOLLYENGINE = 0x1db,
    AER_SAMPLE_ROLLYPOLLYBOUNCE1 = 0x1dc,
    AER_SAMPLE_ROLLYPOLLYBOUNCE2 = 0x1dd,
    AER_SAMPLE_ROLLYPOLLYBOUNCE3 = 0x1de,
    AER_SAMPLE_ROLLYPOLLYBOUNCE4 = 0x1df,
    AER_SAMPLE_ROLLYPOLLYRENEWED = 0x1e0,
    AER_SAMPLE_LASTSHOT = 0x1e1,
    AER_SAMPLE_PISTOLEQUIP = 0x1e2,
    AER_SAMPLE_SHOTGUNEQUIP = 0x1e3,
    AER_SAMPLE_RAILGUNEQUIP = 0x1e4,
    AER_SAMPLE_LIMITBREAKGUNFIRE1 = 0x1e5,
    AER_SAMPLE_LIMITBREAKGUNFIRE2 = 0x1e6,
    AER_SAMPLE_LIMITBREAKGUNFIRE3 = 0x1e7,
    AER_SAMPLE_LIMITBREAKGUNFIRE4 = 0x1e8,
    AER_SAMPLE_FLAMEJAREXPLODE = 0x1e9,
    AER_SAMPLE_CRYSTALLANCETHROW = 0x1ea,
    AER_SAMPLE_CRYSTALLANCEIMPACT = 0x1eb,
    AER_SAMPLE_CRYSTALLANCEEXPLODE = 0x1ec,
    AER_SAMPLE_SWITCHWEAPONS = 0x1ed,
    AER_SAMPLE_WEAPONSWAP = 0x1ee,
    AER_SAMPLE_THROWGRENADE = 0x1ef,
    AER_SAMPLE_PISTOL1 = 0x1f0,
    AER_SAMPLE_PISTOL2 = 0x1f1,
    AER_SAMPLE_PISTOL3 = 0x1f2,
    AER_SAMPLE_RIFLE1 = 0x1f3,
    AER_SAMPLE_RIFLE2 = 0x1f4,
    AER_SAMPLE_BULLETHITWALL = 0x1f5,
    AER_SAMPLE_BULLETHITENEMY = 0x1f6,
    AER_SAMPLE_DIAMONDSHOTGUN1 = 0x1f7,
    AER_SAMPLE_MINEBEEP = 0x1f8,
    AER_SAMPLE_DIAMONDSHOTGUN2 = 0x1f9,
    AER_SAMPLE_ROLLYPOLLYBOUNCE = 0x1fa,
    AER_SAMPLE_CANNONCHARGE = 0x1fb,
    AER_SAMPLE_CANNONLOOP = 0x1fc,
    AER_SAMPLE_RAILGUNFIRE = 0x1fd,
    AER_SAMPLE_RAILGUNFULLYCHARGED = 0x1fe,
    AER_SAMPLE_RAILGUNCHARGE = 0x1ff,
    AER_SAMPLE_RAILGUNFULLCHARGELOOP = 0x200,
    AER_SAMPLE_TWINSHOTCONVERGE = 0x201,
    AER_SAMPLE_TWINSHOTFIRE2 = 0x202,
    AER_SAMPLE_TWINSHOTFIRE1 = 0x203,
    AER_SAMPLE_BLUNDERBUSSFIRE2 = 0x204,
    AER_SAMPLE_BLUNDERBUSSFIRE1 = 0x205,
    AER_SAMPLE_DIAMONDSCONVERGE = 0x206,
    AER_SAMPLE_DIAMONDSCHANGEDIRECTION = 0x207,
    AER_SAMPLE_TRIPLESHOTFIRE3 = 0x208,
    AER_SAMPLE_TRIPLESHOTFIRE2 = 0x209,
    AER_SAMPLE_TRIPLESHOTFIRE1 = 0x20a,
    AER_SAMPLE_INSTASHOTRAILGUN2 = 0x20b,
    AER_SAMPLE_INSTASHOTRAILGUN1 = 0x20c,
    AER_SAMPLE_ZELISKAHITWALL = 0x20d,
    AER_SAMPLE_ZELISKACOCK = 0x20e,
    AER_SAMPLE_ZELISKAFIRE1 = 0x20f,
    AER_SAMPLE_ZELISKAFIRE2 = 0x210,
    AER_SAMPLE_ZELISKAFIRE3 = 0x211,
    AER_SAMPLE_EXPLOSIVERAILGUNCHARGING = 0x212,
    AER_SAMPLE_EXPLOSIVERAILGUNFIRE2 = 0x213,
    AER_SAMPLE_EXPLOSIVERAILGUNFIRE1 = 0x214,
    AER_SAMPLE_EXPLOSIVERAILGUNFULLYCHARGED = 0x215,
    AER_SAMPLE_GRENADETHROWNV2 = 0x216,
    AER_SAMPLE_RAILGUNDRAW = 0x217,
    AER_SAMPLE_FOOTSTEP1V3 = 0x218,
    AER_SAMPLE_FOOTSTEP2V3 = 0x219,
    AER_SAMPLE_FOOTSTEP3V3 = 0x21a,
    AER_SAMPLE_FOOTSTEP4V3 = 0x21b,
    AER_SAMPLE_FOOTSTEPCENTRAL1 = 0x21c,
    AER_SAMPLE_FOOTSTEPCENTRAL2 = 0x21d,
    AER_SAMPLE_FOOTSTEPCENTRAL3 = 0x21e,
    AER_SAMPLE_FOOTSTEPEAST1 = 0x21f,
    AER_SAMPLE_FOOTSTEPEAST2 = 0x220,
    AER_SAMPLE_FOOTSTEPFOREST1 = 0x221,
    AER_SAMPLE_FOOTSTEPEAST3 = 0x222,
    AER_SAMPLE_FOOTSTEPFOREST2 = 0x223,
    AER_SAMPLE_FOOTSTEPFOREST3 = 0x224,
    AER_SAMPLE_FOOTSTEPFOREST4 = 0x225,
    AER_SAMPLE_FOOTSTEPLAB1 = 0x226,
    AER_SAMPLE_FOOTSTEPLAB2 = 0x227,
    AER_SAMPLE_FOOTSTEPLAB3 = 0x228,
    AER_SAMPLE_FOOTSTEPNORTH1 = 0x229,
    AER_SAMPLE_FOOTSTEPNORTH2 = 0x22a,
    AER_SAMPLE_FOOTSTEPNORTH3 = 0x22b,
    AER_SAMPLE_FOOTSTEPSNOW1 = 0x22c,
    AER_SAMPLE_FOOTSTEPSNOW2 = 0x22d,
    AER_SAMPLE_FOOTSTEPSNOW3 = 0x22e,
    AER_SAMPLE_FOOTSTEPSOUTH1 = 0x22f,
    AER_SAMPLE_FOOTSTEPSOUTH2 = 0x230,
    AER_SAMPLE_FOOTSTEPSOUTH3 = 0x231,
    AER_SAMPLE_FOOTSTEPWEST1 = 0x232,
    AER_SAMPLE_FOOTSTEPWEST2 = 0x233,
    AER_SAMPLE_FOOTSTEPWEST3 = 0x234,
    AER_SAMPLE_HEAVYFOOTSTEP1 = 0x235,
    AER_SAMPLE_HEAVYFOOTSTEP2 = 0x236,
    AER_SAMPLE_HEAVYFOOTSTEP3 = 0x237,
    AER_SAMPLE_HEAVYFOOTSTEP4 = 0x238,
    AER_SAMPLE_INVISIBLEPLATFORMFOOTSTEP = 0x239,
    AER_SAMPLE_PUDDLEFOOTSTEP1 = 0x23a,
    AER_SAMPLE_PUDDLEFOOTSTEP2 = 0x23b,
    AER_SAMPLE_PUDDLEFOOTSTEP3 = 0x23c,
    AER_SAMPLE_DRIFTERJUMPDOWN = 0x23d,
    AER_SAMPLE_DRIFTERJUMPDOWNLAND = 0x23e,
    AER_SAMPLE_ALTDRIFTERDASH = 0x23f,
    AER_SAMPLE_DASHV4 = 0x240,
    AER_SAMPLE_DASHCHAIN = 0x241,
    AER_SAMPLE_DASH = 0x242,
    AER_SAMPLE_ABSORBDASH = 0x243,
    AER_SAMPLE_BREAKDASH = 0x244,
    AER_SAMPLE_WALLDASH = 0x245,
    AER_SAMPLE_ALTDRIFTERGRUNT1 = 0x246,
    AER_SAMPLE_ALTDRIFTERGRUNT2 = 0x247,
    AER_SAMPLE_ALTDRIFTERGRUNT3 = 0x248,
    AER_SAMPLE_ALTDRIFTERDEATH = 0x249,
    AER_SAMPLE_ALTDRIFTERHURTLIGHT = 0x24a,
    AER_SAMPLE_ALTDRIFTERHURTHEAVY = 0x24b,
    AER_SAMPLE_DRIFTERCAUGHTINCRYSTAL = 0x24c,
    AER_SAMPLE_DRIFTERHURT1DMG = 0x24d,
    AER_SAMPLE_DRIFTERHURT2DMG = 0x24e,
    AER_SAMPLE_DRIFTERHURT3DMG = 0x24f,
    AER_SAMPLE_HEALPIP = 0x250,
    AER_SAMPLE_EXPLOSIONV2 = 0x251,
    AER_SAMPLE_SHIELDHITV2 = 0x252,
    AER_SAMPLE_CHARGETHIT = 0x253,
    AER_SAMPLE_SHIELD = 0x254,
    AER_SAMPLE_SHIELDACTIVATEDV2 = 0x255,
    AER_SAMPLE_SHIELDACTIVATEDV3 = 0x256,
    AER_SAMPLE_CHARGETHITOLD = 0x257,
    AER_SAMPLE_DRIFTERCOUGH1 = 0x258,
    AER_SAMPLE_DRIFTERCOUGH2 = 0x259,
    AER_SAMPLE_DRIFTERCOUGH3 = 0x25a,
    AER_SAMPLE_FASTTRAVELPANEL = 0x25b,
    AER_SAMPLE_FASTTRAVELDEPART = 0x25c,
    AER_SAMPLE_FASTTRAVELARRIVE = 0x25d,
    AER_SAMPLE_LIGHTSHUTDOWN = 0x25e,
    AER_SAMPLE_TOWERPULSE = 0x25f,
    AER_SAMPLE_TOWERINITIALRUMBLE = 0x260,
    AER_SAMPLE_TOWERRISING = 0x261,
    AER_SAMPLE_TOWERFINISHRISING = 0x262,
    AER_SAMPLE_ABYSSDOORSTOP = 0x263,
    AER_SAMPLE_ABYSSDOOROPENING = 0x264,
    AER_SAMPLE_ABYSSTELEVATORMOVE = 0x265,
    AER_SAMPLE_ABYSSTELEVATORSOCKET = 0x266,
    AER_SAMPLE_BIRDFLYAWAY = 0x267,
    AER_SAMPLE_BONESMASH = 0x268,
    AER_SAMPLE_CHECKPOINTAMBIENTLOOP = 0x269,
    AER_SAMPLE_CHECKPOINTSAVE = 0x26a,
    AER_SAMPLE_CUTSHRUB = 0x26b,
    AER_SAMPLE_DOORLABLATCHDOWN = 0x26c,
    AER_SAMPLE_DOORLABLATCHUP = 0x26d,
    AER_SAMPLE_DOOROPENV2 = 0x26e,
    AER_SAMPLE_FLAMELOOP = 0x26f,
    AER_SAMPLE_FLAMEOFF = 0x270,
    AER_SAMPLE_FLAMEON = 0x271,
    AER_SAMPLE_FLOORSWITCH = 0x272,
    AER_SAMPLE_GLASSSHATTER = 0x273,
    AER_SAMPLE_HEARTBEATV2 = 0x274,
    AER_SAMPLE_INVISIBLEPLATFORMSCAN = 0x275,
    AER_SAMPLE_METALDESTROY = 0x276,
    AER_SAMPLE_MODULECAPSULEOPEN = 0x277,
    AER_SAMPLE_MODULEPING = 0x278,
    AER_SAMPLE_MOVINGPLATFORM = 0x279,
    AER_SAMPLE_PILLARSWITCHPING = 0x27a,
    AER_SAMPLE_ROCKDESTROY = 0x27b,
    AER_SAMPLE_SMASHCRATE = 0x27c,
    AER_SAMPLE_SOCKETBLOCKGODOWN = 0x27d,
    AER_SAMPLE_SOCKETBLOCKHITTOP = 0x27e,
    AER_SAMPLE_SOCKETBLOCKRISE = 0x27f,
    AER_SAMPLE_SPLASH = 0x280,
    AER_SAMPLE_TELEPORTIN = 0x281,
    AER_SAMPLE_TELEVATORMOVE = 0x282,
    AER_SAMPLE_TELEVATORSOCKET = 0x283,
    AER_SAMPLE_THUNDER1 = 0x284,
    AER_SAMPLE_THUNDER2 = 0x285,
    AER_SAMPLE_THUNDER3 = 0x286,
    AER_SAMPLE_TITANHEARTBEAT = 0x287,
    AER_SAMPLE_TONETOWERCORRUPTEDLOOP = 0x288,
    AER_SAMPLE_TONETOWERLASER = 0x289,
    AER_SAMPLE_TONETOWERLOOP = 0x28a,
    AER_SAMPLE_TONETOWERSTRUCK = 0x28b,
    AER_SAMPLE_TONETOWERSTRUCKCORRUPTED = 0x28c,
    AER_SAMPLE_WATERSPLASH = 0x28d,
    AER_SAMPLE_DEBUGBEEP = 0x28e,
    AER_SAMPLE_SHACKJACK = 0x28f,
    AER_SAMPLE_TANUKISLASH = 0x290,
    AER_SAMPLE_BATTERYPICKUP = 0x291,
    AER_SAMPLE_COLLECTDRIFTERKEY = 0x292,
    AER_SAMPLE_COLLECTGEARBIT = 0x293,
    AER_SAMPLE_COLLECTMAJOR = 0x294,
    AER_SAMPLE_COLLECTLIBRARIANTABLET = 0x295,
    AER_SAMPLE_COLLECTMINOR = 0x296,
    AER_SAMPLE_COLLECTMODULE = 0x297,
    AER_SAMPLE_COLLECTMODULE_V2 = 0x298,
    AER_SAMPLE_COLLECTTOWER = 0x299,
    AER_SAMPLE_COLLECTWEAPON = 0x29a,
    AER_SAMPLE_GENERICCOLLECTABLE = 0x29b,
    AER_SAMPLE_GENERICCOLLECTABLEOLD = 0x29c,
    AER_SAMPLE_GENERICCOLLECTIBLE2 = 0x29d,
    AER_SAMPLE_HEALTHPICKUP = 0x29e,
    AER_SAMPLE_HEALTHUSE = 0x29f,
    AER_SAMPLE_HEALTHWASTE = 0x2a0,
    AER_SAMPLE_PICKUPENERGY_V2 = 0x2a1,
    AER_SAMPLE_PICKUPITEM = 0x2a2,
    AER_SAMPLE_SPECIALAMMOPICKUP = 0x2a3,
    AER_SAMPLE_DRIFTERSAVEDATNIGHT = 0x2a4
} AERAudioSampleIndex;

/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERAudioSampleRegister(const char* filename, const char* name);

size_t AERAudioSampleGetNumRegistered(void);

int32_t AERAudioSampleGetByName(const char* name);

const char* AERAudioSampleGetName(int32_t sampleIdx);

int32_t AERAudioSamplePlay(int32_t sampleIdx, double priority, bool loop);

// int32_t AERAudioSamplePlayAt(int32_t sampleIdx,
//                              double x,
//                              double y,
//                              double falloffRef,
//                              double falloffMax,
//                              double priority,
//                              bool loop);

// bool AERAudioSampleAnyActive(int32_t sampleIdx);

// bool AERAudioPlaybackIsActive(int32_t playbackId);

// bool AERAudioSampleAnyPaused(int32_t sampleIdx);

// bool AERAudioPlaybackIsPaused(int32_t playbackId);

// void AERAudioStopAll(void);

// void AERAudioSampleStopAll(int32_t sampleIdx);

// void AERAudioPlaybackStop(int32_t playbackId);

// void AERAudioPauseAll(void);

// void AERAudioSamplePauseAll(int32_t sampleIdx);

// void AERAudioPlaybackPause(int32_t playbackId);

// void AERAudioResumeAll(void);

// void AERAudioSampleResumeAll(int32_t sampleIdx);

// void AERAudioPlaybackResume(int32_t playbackId);

#endif /* AER_AUDIO_H */