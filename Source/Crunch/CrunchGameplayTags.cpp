#include "CrunchGameplayTags.h"
#include "GameplayTagsManager.h"

namespace CrunchGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Passive_Launch_Activate, "Ability.Passive.Launch.Activate",
	                               "Tag for activating the passive launch ability. This tag is used to trigger the passive launch ability in the game.");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Dead, "Stats.Dead", "死亡的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Stun, "Stats.Stun", "眩晕的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Aim, "Stats.Aim", "瞄准的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Crosshair, "Stats.Crosshair", "瞄准有准星的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Health_Full, "Stats.Health.Full", "生命为满的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Health_Empty, "Stats.Health.Empty", "生命为空的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Mana_Full, "Stats.Mana.Full", "Mana为满的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Mana_Empty, "Stats.Mana.Empty", "Mana为空的状态")


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Role_Hero, "Role.Hero", "代表这是一个Hero");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attr_Experience, "Attr.Experience", "代表属性中经验的Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attr_Gold, "Attr.Gold", "代表属性中金币的Tag");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack, "Ability.BasicAttack", "Tag for the basic attack ability. This tag is used to identify the basic attack ability in the game.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack_Pressed, "Ability.BasicAttack.Pressed",
	                               "Tag used for event that will be signed when the basic attack input is pressed. ");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack_Released, "Ability.BasicAttack.Released",
	                               "Tag used for event that will be signed when the basic attack input is Released. ");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Damage, "Ability.Combo.Damage",
	                               "Tag for the combo damage ability. This tag is used to identify the combo damage ability in the game.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo01, "Ability.Combo.Change.Combo01", "Tag for the first combo change in the combo ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo02, "Ability.Combo.Change.Combo02", "Tag for the second combo change in the combo ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo03, "Ability.Combo.Change.Combo03", "Tag for the third combo change in the combo ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_Combo04, "Ability.Combo.Change.Combo04", "Tag for the fourth combo change in the combo ability.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Combo_Change_End, "Ability.Combo.Change.End", "Tag for ending the combo ability. This tag is used to signal the end of a combo sequence.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_UpperCut_Launch, "Ability.UpperCut.Launch", "Tag for UpperCut ability launch event. This tag is used to trigger the UpperCut ability in the game.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_UpperCut_Cooldown, "Ability.UpperCut.Cooldown",
	                               "Tag for UpperCut ability cooldown. This tag is used to manage the cooldown state of the UpperCut ability.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_GroundBlast_Cooldown, "Ability.GroundBlast.Cooldown",
							   "Tag for GroundBlast ability cooldown. This tag is used to manage the cooldown state of the GroundBlast ability.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Shoot, "Ability.Shoot", "The Ability which Shoot Projectile");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Shoot_LeftHand, "Ability.Shoot.LeftHand", "Shoot With Left Hand");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Shoot_RightHand, "Ability.Shoot.RightHand", "Shoot With Right Hand");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Target_Updated, "Target.Updated", "代表目标已经更新的Tag,用于本地更新准星")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Lazer_Shoot, "Ability.Lazer.Shoot", "Phase人物的激光射击能力的Tag.")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Lazer_Cooldown, "Ability.Lazer.Cooldown", "Phase人物的激光射击能力Cooldown的Tag.")


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_CameraShake, "GameplayCue.CameraShake", "屏幕震动的GameplayCue")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_GroundBlast, "GameplayCue.GroundBlast", "地面爆炸的GameplayCue");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Test_Cooldown, "Ability.Test.Cooldown", "用作测试的Cooldown Tag");


	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					//@todo: 创建自己的Log类.
					//UE_LOG(LogLyra, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}
