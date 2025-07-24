#include "CrunchGameplayTags.h"
#include "GameplayTagsManager.h"

namespace CrunchGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Passive_Launch_Activate, "Ability.Passive.Launch.Activate",
	                               "Tag for activating the passive launch ability. This tag is used to trigger the passive launch ability in the game.");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Dead, "Stats.Dead", "死亡的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Stun, "Stats.Stun", "眩晕的状态")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stats_Aim, "Stats.Aim", "瞄准的状态")

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack, "Ability.BasicAttack", "Tag for the basic attack ability. This tag is used to identify the basic attack ability in the game.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_BasicAttack_Pressed, "Ability.BasicAttack.Pressed",
	                               "Tag used for event that will be signed when the basic attack input is pressed. ");

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


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_CameraShake, "GameplayCue.CameraShake", "屏幕震动的GameplayCue")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_GroundBlast, "GameplayCue.GroundBlast", "地面爆炸的GameplayCue");

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
