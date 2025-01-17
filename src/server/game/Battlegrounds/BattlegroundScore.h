/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRINITY_BATTLEGROUND_SCORE_H
#define TRINITY_BATTLEGROUND_SCORE_H

#include "WorldPacket.h"
#include "Player.h"
#include "ObjectAccessor.h"

enum ScoreType
{
    SCORE_KILLING_BLOWS         = 1,
    SCORE_DEATHS                = 2,
    SCORE_HONORABLE_KILLS       = 3,
    SCORE_BONUS_HONOR           = 4,
    SCORE_DAMAGE_DONE           = 5,
    SCORE_HEALING_DONE          = 6,

    // WS, EY and TP
    SCORE_FLAG_CAPTURES         = 7,
    SCORE_FLAG_RETURNS          = 8,

    // AB and IC
    SCORE_BASES_ASSAULTED       = 9,
    SCORE_BASES_DEFENDED        = 10,

    // AV
    SCORE_GRAVEYARDS_ASSAULTED  = 11,
    SCORE_GRAVEYARDS_DEFENDED   = 12,
    SCORE_TOWERS_ASSAULTED      = 13,
    SCORE_TOWERS_DEFENDED       = 14,
    SCORE_MINES_CAPTURED        = 15,

    // SOTA
    SCORE_DESTROYED_DEMOLISHER  = 16,
    SCORE_DESTROYED_WALL        = 17
};

struct BattlegroundScore
{
    friend class Arena;
    friend class Battleground;

    protected:
        BattlegroundScore(ObjectGuid playerGuid, uint32 team) : PlayerGuid(playerGuid), TeamId(team == ALLIANCE ? 1 : 0),
            KillingBlows(0), Deaths(0), HonorableKills(0), BonusHonor(0), DamageDone(0), HealingDone(0) { }

        virtual ~BattlegroundScore() { }

        virtual void UpdateScore(uint32 type, uint32 value)
        {
            switch (type)
            {
                case SCORE_KILLING_BLOWS:   // Killing blows
                    KillingBlows += value;
                    break;
                case SCORE_DEATHS:          // Deaths
                    Deaths += value;
                    break;
                case SCORE_HONORABLE_KILLS: // Honorable kills
                    HonorableKills += value;
                    break;
                case SCORE_BONUS_HONOR:     // Honor bonus
                    BonusHonor += value;
                    break;
                case SCORE_DAMAGE_DONE:     // Damage Done
                    DamageDone += value;
                    break;
                case SCORE_HEALING_DONE:    // Healing Done
                    HealingDone += value;
                    break;
                default:
                    ASSERT(false && "Not implemented Battleground score type!");
                    break;
            }
        }

        virtual void AppendToPacket(WorldPacket& data, ByteBuffer& content)
        {
            uint32 primaryTree = 0;
            if (Player* player = ObjectAccessor::FindPlayer(PlayerGuid))
                primaryTree = player->GetPrimaryTalentTree(player->GetActiveSpec());

            data.WriteBit(0);                   // Unk 1
            data.WriteBit(0);                   // Unk 2
            data.WriteBit(PlayerGuid[2]);
            data.WriteBit(/*!IsArena*/ 1);      // IsArena
            data.WriteBit(0);                   // Unk 4
            data.WriteBit(0);                   // Unk 5
            data.WriteBit(0);                   // Unk 6
            data.WriteBit(PlayerGuid[3]);
            data.WriteBit(PlayerGuid[0]);
            data.WriteBit(PlayerGuid[5]);
            data.WriteBit(PlayerGuid[1]);
            data.WriteBit(PlayerGuid[6]);
            data.WriteBit(TeamId);
            data.WriteBit(PlayerGuid[7]);

            content << uint32(HealingDone);     // healing done
            content << uint32(DamageDone);      // damage done

            //if (!IsArena)
            //{
                content << uint32(BonusHonor / 100);
                content << uint32(Deaths);
                content << uint32(HonorableKills);
            //}

            content.WriteByteSeq(PlayerGuid[4]);
            content << uint32(KillingBlows);

            //if (unk 5)
            //    data << uint32() unk

            content.WriteByteSeq(PlayerGuid[5]);

            //if (unk 6)
            //    data << uint32() unk

            //if (unk 2)
            //    data << uint32() unk

            content.WriteByteSeq(PlayerGuid[1]);
            content.WriteByteSeq(PlayerGuid[6]);

            content << int32(primaryTree);

            BuildObjectivesBlock(data, content);

            data.WriteBit(PlayerGuid[4]);

            content.WriteByteSeq(PlayerGuid[0]);
            content.WriteByteSeq(PlayerGuid[3]);

            //if (unk 4)
            //    data << uint32() unk

            content.WriteByteSeq(PlayerGuid[7]);
            content.WriteByteSeq(PlayerGuid[2]);
        }

        virtual void BuildObjectivesBlock(WorldPacket& /*data*/, ByteBuffer& /*content*/) = 0;

        // For Logging purpose
        virtual std::string ToString() const { return ""; }

        uint32 GetKillingBlows() const    { return KillingBlows; }
        uint32 GetDeaths() const          { return Deaths; }
        uint32 GetHonorableKills() const  { return HonorableKills; }
        uint32 GetBonusHonor() const      { return BonusHonor; }
        uint32 GetDamageDone() const      { return DamageDone; }
        uint32 GetHealingDone() const     { return HealingDone; }

        virtual uint32 GetAttr1() const { return 0; }
        virtual uint32 GetAttr2() const { return 0; }
        virtual uint32 GetAttr3() const { return 0; }
        virtual uint32 GetAttr4() const { return 0; }
        virtual uint32 GetAttr5() const { return 0; }

        ObjectGuid PlayerGuid;
        uint8 TeamId;

        // Default score, present in every type
        uint32 KillingBlows;
        uint32 Deaths;
        uint32 HonorableKills;
        uint32 BonusHonor;
        uint32 DamageDone;
        uint32 HealingDone;
};

#endif // TRINITY_BATTLEGROUND_SCORE_H
