// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <pch.h>
#include "Social_Integration.h"
#include "XSAPI_Integration.h"

#pragma region Social Integration

void Social_AddUserToSocialManager(
    _In_ XalUserHandle user)
{
    HRESULT hr = XblSocialManagerAddLocalUser(user, XblSocialManagerExtraDetailLevel::All, nullptr);
    
    if (FAILED(hr))
    {
        SampleLog(LL_ERROR, "XblSocialManagerAddLocalUser Failed!");
        SampleLog(LL_ERROR, "Error code: %s", ConvertHRtoString(hr).c_str());
        return;
    }

    SampleLog(LL_TRACE, "Successfully added local user to Social Manager"); // TODO: add user info?
    
    //Setup the social groups.
    XblSocialManagerUserGroup* friendsSocialGroup = Social_CreateSocialGroupFromFilters(
        user, XblPresenceFilter::All, XblRelationshipFilter::Friends);
    XblSocialManagerUserGroup* favoriteSocialGroup = Social_CreateSocialGroupFromFilters(
        user, XblPresenceFilter::All, XblRelationshipFilter::Favorite);
    
    Social_Gameplay_UpdateFriendsSocialGroup(friendsSocialGroup);
    Social_Gameplay_UpdateFavoriteSocialGroup(favoriteSocialGroup);
}

void Social_RemoveUserFromSocialManager(
    _In_ XalUserHandle user)
{
    HRESULT hr = XblSocialManagerRemoveLocalUser(user);

    if (FAILED(hr))
    {
        SampleLog(LL_ERROR, "XblSocialManagerRemoveLocalUser Failed!");
        SampleLog(LL_ERROR, "Error code: %s", ConvertHRtoString(hr).c_str());
        return;
    }

    SampleLog(LL_TRACE, "Successfully removed local user to Social Manager"); // TODO: add user info?
    
    Social_Gameplay_UpdateFriendsSocialGroup(nullptr);
    Social_Gameplay_UpdateFavoriteSocialGroup(nullptr);
}

void Social_GetUsersForSocialGroup(
    _In_ XblSocialManagerUserGroup* group,
    _In_ size_t xboxSocialUsersCount,
    _Out_writes_ (xboxSocialUserCount) XblSocialManagerUserPtrArray xboxSocialUsers)
{
    HRESULT hr = XblSocialManagerUserGroupGetUsers(group, &xboxSocialUsers, &xboxSocialUsersCount);

    if (FAILED(hr))
    {
        SampleLog(LL_ERROR, "XblSocialManagerUserGroupGetUsers Failed!");
        SampleLog(LL_ERROR, "Error code: %s", ConvertHRtoString(hr).c_str());
        return;
    }

    SampleLog(LL_TRACE, "Successfully grabbed users for a social group.");
}

XblSocialManagerUserGroup* Social_CreateSocialGroupFromList(
    _In_ XalUserHandle user,
    _In_ uint64_t* xboxUserIdList,
    _In_ uint32_t xboxUserIdListCount)
{
     if (xboxUserIdList == nullptr || xboxUserIdListCount == 0)
     {
        SampleLog(LL_WARNING, "Cannot create social group with an empty user list.");
        return nullptr;
     }
     
    XblSocialManagerUserGroup* group = nullptr;
    
    HRESULT hr = XblSocialManagerCreateSocialUserGroupFromList(user, xboxUserIdList, xboxUserIdListCount, &group);
    
    if (FAILED(hr))
    {
        SampleLog(LL_ERROR, "XblSocialManagerCreateSocialUserGroupFromList Failed!");
        SampleLog(LL_ERROR, "Error code: %s", ConvertHRtoString(hr).c_str());
        return nullptr;
    }
    
    SampleLog(LL_TRACE, "Successfully created social group from user list");
    return group;
}

XblSocialManagerUserGroup* Social_CreateSocialGroupFromFilters(
    _In_ XalUserHandle user,
    _In_ XblPresenceFilter presenceDetailLevel,
    _In_ XblRelationshipFilter filter)
{
    XblSocialManagerUserGroup* group = nullptr;
    
    HRESULT hr = XblSocialManagerCreateSocialUserGroupFromFilters(user, presenceDetailLevel, filter, &group);
    
    if (FAILED(hr))
    {
        SampleLog(LL_ERROR, "XblSocialManagerCreateSocialUserGroupFromFilters Failed!");
        SampleLog(LL_ERROR, "Error code: %s", ConvertHRtoString(hr).c_str());
        return nullptr;
    }
    
    SampleLog(LL_TRACE, "Successfully created social group from filters");
    return group;
}

void Social_UpdateSocialManager()
{
    const XblSocialManagerEvent* events { nullptr };
    size_t eventCount = 0;
    
    HRESULT hr = XblSocialManagerDoWork(&events, &eventCount);
    
    if (FAILED(hr))
    {
        SampleLog(LL_ERROR, "XblSocialManagerDoWork Failed!");
        SampleLog(LL_ERROR, "Error code: %s", ConvertHRtoString(hr).c_str());
        return;
    }

    bool doRefresh = eventCount > 0;    // For now, refresh Social on any event.
    if (doRefresh) {
        Social_Gameplay_RefreshSocialGroups();
    }
}

#pragma endregion
