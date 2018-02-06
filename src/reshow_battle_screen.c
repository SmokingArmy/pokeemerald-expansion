#include "global.h"
#include "reshow_battle_screen.h"
#include "battle.h"
#include "palette.h"
#include "main.h"
#include "scanline_effect.h"
#include "text.h"
#include "gpu_regs.h"
#include "bg.h"
#include "battle_controllers.h"
#include "link.h"
#include "sprite.h"
#include "constants/species.h"
#include "battle_interface.h"

extern u16 gBattle_BG0_X;
extern u16 gBattle_BG0_Y;
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;
extern u16 gBattle_BG3_X;
extern u16 gBattle_BG3_Y;
extern u8 gReservedSpritePaletteCount;
extern u8 gActionSelectionCursor[MAX_BATTLERS_COUNT];
extern u8 gBankInMenu;
extern u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
extern u8 gBattlersCount;
extern u32 gBattleTypeFlags;
extern u8 gBattlerSpriteIds[MAX_BATTLERS_COUNT];
extern u8 gBattleMonForms[MAX_BATTLERS_COUNT];
extern u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
extern struct SpriteTemplate gUnknown_0202499C;

extern const union AnimCmd * const * const gMonAnimationsSpriteAnimsPtrTable[];

extern void ScanlineEffect_Clear(void);
extern void sub_8035658(void);
extern bool8 IsDoubleBattle(void);
extern u8 GetSubstituteSpriteDefault_Y(u8 bank);
extern u8 GetBattlerSpriteDefault_Y(u8 bank);
extern u8 sub_80A82E4(u8 bank);
extern void sub_806A068(u16 species, u8 bankIdentity);
extern void sub_806A12C(u16 backPicId, u8 bankIdentity);
extern u8 GetBattlerSpriteCoord(u8 bank, u8 caseId);

// this file's functions
static void CB2_ReshowBattleScreenAfterMenu(void);
static bool8 LoadBankSpriteGfx(u8 bank);
static void CreateBankSprite(u8 bank);
static void CreateHealthboxSprite(u8 bank);
static void sub_80A95F4(void);

void nullsub_35(void)
{

}

void ReshowBattleScreenAfterMenu(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    SetHBlankCallback(NULL);
    SetVBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_MOSAIC, 0);
    gBattleScripting.reshowMainState = 0;
    gBattleScripting.reshowHelperState = 0;
    SetMainCallback2(CB2_ReshowBattleScreenAfterMenu);
}

static void CB2_ReshowBattleScreenAfterMenu(void)
{
    switch (gBattleScripting.reshowMainState)
    {
    case 0:
        ScanlineEffect_Clear();
        sub_8035658();
        SetBgAttribute(1, BG_CTRL_ATTR_VISIBLE, 0);
        SetBgAttribute(2, BG_CTRL_ATTR_VISIBLE, 0);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        ShowBg(3);
        ResetPaletteFade();
        gBattle_BG0_X = 0;
        gBattle_BG0_Y = 0;
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
        gBattle_BG2_X = 0;
        gBattle_BG2_Y = 0;
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        break;
    case 1:
        CpuFastFill(0, (void*)(VRAM), VRAM_SIZE);
        break;
    case 2:
        LoadBattleTextboxAndBackground();
        break;
    case 3:
        ResetSpriteData();
        break;
    case 4:
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 4;
        break;
    case 5:
        ClearSpritesHealthboxAnimData();
        break;
    case 6:
        if (BattleLoadAllHealthBoxesGfx(gBattleScripting.reshowHelperState))
        {
            gBattleScripting.reshowHelperState = 0;
        }
        else
        {
            gBattleScripting.reshowHelperState++;
            gBattleScripting.reshowMainState--;
        }
        break;
    case 7:
        if (!LoadBankSpriteGfx(0))
            gBattleScripting.reshowMainState--;
        break;
    case 8:
        if (!LoadBankSpriteGfx(1))
            gBattleScripting.reshowMainState--;
        break;
    case 9:
        if (!LoadBankSpriteGfx(2))
            gBattleScripting.reshowMainState--;
        break;
    case 10:
        if (!LoadBankSpriteGfx(3))
            gBattleScripting.reshowMainState--;
        break;
    case 11:
        CreateBankSprite(0);
        break;
    case 12:
        CreateBankSprite(1);
        break;
    case 13:
        CreateBankSprite(2);
        break;
    case 14:
        CreateBankSprite(3);
        break;
    case 15:
        CreateHealthboxSprite(0);
        break;
    case 16:
        CreateHealthboxSprite(1);
        break;
    case 17:
        CreateHealthboxSprite(2);
        break;
    case 18:
        CreateHealthboxSprite(3);
        break;
    case 19:
        {
            u8 opponentBank;
            u16 species;

            LoadAndCreateEnemyShadowSprites();

            opponentBank = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[opponentBank]], MON_DATA_SPECIES);
            SetBattlerShadowSpriteCallback(opponentBank, species);

            if (IsDoubleBattle())
            {
                opponentBank = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
                species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[opponentBank]], MON_DATA_SPECIES);
                SetBattlerShadowSpriteCallback(opponentBank, species);
            }

            ActionSelectionCreateCursorAt(gActionSelectionCursor[gBankInMenu], 0);

            if (gLinkVSyncDisabled != 0 && gReceivedRemoteLinkPlayers != 0)
            {
                sub_800E0E8();
                sub_800DFB4(0, 0);
            }
        }
        break;
    default:
        SetVBlankCallback(VBlankCB_Battle);
        sub_80A95F4();
        BeginHardwarePaletteFade(0xFF, 0, 0x10, 0, 1);
        gPaletteFade.bufferTransferDisabled = 0;
        SetMainCallback2(BattleMainCB2);
        sub_805EF14();
        break;
    }

    gBattleScripting.reshowMainState++;
}

static void sub_80A95F4(void)
{
    struct BGCntrlBitfield *regBgcnt1, *regBgcnt2;

    regBgcnt1 = (struct BGCntrlBitfield *)(&REG_BG1CNT);
    regBgcnt1->charBaseBlock = 0;

    regBgcnt2 = (struct BGCntrlBitfield *)(&REG_BG2CNT);
    regBgcnt2->charBaseBlock = 0;
}

static bool8 LoadBankSpriteGfx(u8 bank)
{
    if (bank < gBattlersCount)
    {
        if (GetBattlerSide(bank) != B_SIDE_PLAYER)
        {
            if (!gBattleSpritesDataPtr->bankData[bank].behindSubstitute)
                BattleLoadOpponentMonSpriteGfx(&gEnemyParty[gBattlerPartyIndexes[bank]], bank);
            else
                BattleLoadSubstituteOrMonSpriteGfx(bank, FALSE);
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && bank == 0)
            DecompressTrainerBackPic(gSaveBlock2Ptr->playerGender, bank);
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && bank == 0)
            DecompressTrainerBackPic(BACK_PIC_WALLY, bank);
        else if (!gBattleSpritesDataPtr->bankData[bank].behindSubstitute)
            BattleLoadPlayerMonSpriteGfx(&gPlayerParty[gBattlerPartyIndexes[bank]], bank);
        else
            BattleLoadSubstituteOrMonSpriteGfx(bank, FALSE);

        gBattleScripting.reshowHelperState = 0;
    }
    return TRUE;
}

// todo: get rid of it once the struct is declared in a header
struct MonCoords
{
    // This would use a bitfield, but sub_8079F44
    // uses it as a u8 and casting won't match.
    u8 coords; // u8 x:4, y:4;
    u8 y_offset;
};
extern const struct MonCoords gTrainerBackPicCoords[];

static void CreateBankSprite(u8 bank)
{
    if (bank < gBattlersCount)
    {
        u8 posY;

        if (gBattleSpritesDataPtr->bankData[bank].behindSubstitute)
            posY = GetSubstituteSpriteDefault_Y(bank);
        else
            posY = GetBattlerSpriteDefault_Y(bank);

        if (GetBattlerSide(bank) != B_SIDE_PLAYER)
        {
            if (GetMonData(&gEnemyParty[gBattlerPartyIndexes[bank]], MON_DATA_HP) == 0)
                return;

            sub_806A068(GetMonData(&gEnemyParty[gBattlerPartyIndexes[bank]], MON_DATA_SPECIES), GetBattlerPosition(bank));
            gBattlerSpriteIds[bank] = CreateSprite(&gUnknown_0202499C, GetBattlerSpriteCoord(bank, 2), posY, sub_80A82E4(bank));
            gSprites[gBattlerSpriteIds[bank]].oam.paletteNum = bank;
            gSprites[gBattlerSpriteIds[bank]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[bank]].data[0] = bank;
            gSprites[gBattlerSpriteIds[bank]].data[2] = GetMonData(&gEnemyParty[gBattlerPartyIndexes[bank]], MON_DATA_SPECIES);

            StartSpriteAnim(&gSprites[gBattlerSpriteIds[bank]], gBattleMonForms[bank]);
            if (gBattleSpritesDataPtr->bankData[bank].transformSpecies == SPECIES_CASTFORM)
                gSprites[gBattlerSpriteIds[bank]].anims = gMonAnimationsSpriteAnimsPtrTable[SPECIES_CASTFORM];
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && bank == 0)
        {
            sub_806A12C(gSaveBlock2Ptr->playerGender, GetBattlerPosition(B_POSITION_PLAYER_LEFT));
            gBattlerSpriteIds[bank] = CreateSprite(&gUnknown_0202499C, 0x50,
                                                (8 - gTrainerBackPicCoords[gSaveBlock2Ptr->playerGender].coords) * 4 + 80,
                                                 sub_80A82E4(0));
            gSprites[gBattlerSpriteIds[bank]].oam.paletteNum = bank;
            gSprites[gBattlerSpriteIds[bank]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[bank]].data[0] = bank;
        }
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && bank == 0)
        {
            sub_806A12C(BACK_PIC_WALLY, GetBattlerPosition(0));
            gBattlerSpriteIds[bank] = CreateSprite(&gUnknown_0202499C, 0x50,
                                                (8 - gTrainerBackPicCoords[BACK_PIC_WALLY].coords) * 4 + 80,
                                                 sub_80A82E4(0));
            gSprites[gBattlerSpriteIds[bank]].oam.paletteNum = bank;
            gSprites[gBattlerSpriteIds[bank]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[bank]].data[0] = bank;
        }
        else
        {
            if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[bank]], MON_DATA_HP) == 0)
                return;

            sub_806A068(GetMonData(&gPlayerParty[gBattlerPartyIndexes[bank]], MON_DATA_SPECIES), GetBattlerPosition(bank));
            gBattlerSpriteIds[bank] = CreateSprite(&gUnknown_0202499C, GetBattlerSpriteCoord(bank, 2), posY, sub_80A82E4(bank));
            gSprites[gBattlerSpriteIds[bank]].oam.paletteNum = bank;
            gSprites[gBattlerSpriteIds[bank]].callback = SpriteCallbackDummy;
            gSprites[gBattlerSpriteIds[bank]].data[0] = bank;
            gSprites[gBattlerSpriteIds[bank]].data[2] = GetMonData(&gPlayerParty[gBattlerPartyIndexes[bank]], MON_DATA_SPECIES);

            StartSpriteAnim(&gSprites[gBattlerSpriteIds[bank]], gBattleMonForms[bank]);
            if (gBattleSpritesDataPtr->bankData[bank].transformSpecies == SPECIES_CASTFORM)
                gSprites[gBattlerSpriteIds[bank]].anims = gMonAnimationsSpriteAnimsPtrTable[SPECIES_CASTFORM];
        }

        gSprites[gBattlerSpriteIds[bank]].invisible = gBattleSpritesDataPtr->bankData[bank].invisible;
    }
}

static void CreateHealthboxSprite(u8 bank)
{
    if (bank < gBattlersCount)
    {
        u8 healthboxSpriteId;

        if (gBattleTypeFlags & BATTLE_TYPE_SAFARI && bank == 0)
            healthboxSpriteId = CreateSafariPlayerHealthboxSprites();
        else if (gBattleTypeFlags & BATTLE_TYPE_WALLY_TUTORIAL && bank == 0)
            return;
        else
            healthboxSpriteId = CreateHealthboxSprites(bank);

        gHealthboxSpriteIds[bank] = healthboxSpriteId;
        InitBattlerHealthboxCoords(bank);
        SetHealthboxSpriteVisible(healthboxSpriteId);

        if (GetBattlerSide(bank) != B_SIDE_PLAYER)
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank], &gEnemyParty[gBattlerPartyIndexes[bank]], HEALTHBOX_ALL);
        else if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank], &gPlayerParty[gBattlerPartyIndexes[bank]], HEALTHBOX_SAFARI_ALL_TEXT);
        else
            UpdateHealthboxAttribute(gHealthboxSpriteIds[bank], &gPlayerParty[gBattlerPartyIndexes[bank]], HEALTHBOX_ALL);

        if (GetBattlerPosition(bank) == B_POSITION_OPPONENT_RIGHT || GetBattlerPosition(bank) == B_POSITION_PLAYER_RIGHT)
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[bank], TRUE);
        else
            DummyBattleInterfaceFunc(gHealthboxSpriteIds[bank], FALSE);

        if (GetBattlerSide(bank) != B_SIDE_PLAYER)
        {
            if (GetMonData(&gEnemyParty[gBattlerPartyIndexes[bank]], MON_DATA_HP) == 0)
                SetHealthboxSpriteInvisible(healthboxSpriteId);
        }
        else if (!(gBattleTypeFlags & BATTLE_TYPE_SAFARI))
        {
            if (GetMonData(&gPlayerParty[gBattlerPartyIndexes[bank]], MON_DATA_HP) == 0)
                SetHealthboxSpriteInvisible(healthboxSpriteId);
        }
    }
}
