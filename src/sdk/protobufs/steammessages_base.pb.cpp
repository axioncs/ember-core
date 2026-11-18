

#include "steammessages_base.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
constexpr CMsgIPAddress::CMsgIPAddress(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : _oneof_case_{}{}
struct CMsgIPAddressDefaultTypeInternal {
  constexpr CMsgIPAddressDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgIPAddressDefaultTypeInternal() {}
  union {
    CMsgIPAddress _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgIPAddressDefaultTypeInternal _CMsgIPAddress_default_instance_;
constexpr CMsgIPAddressBucket::CMsgIPAddressBucket(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : original_ip_address_(nullptr)
  , bucket_(PROTOBUF_ULONGLONG(0)){}
struct CMsgIPAddressBucketDefaultTypeInternal {
  constexpr CMsgIPAddressBucketDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgIPAddressBucketDefaultTypeInternal() {}
  union {
    CMsgIPAddressBucket _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgIPAddressBucketDefaultTypeInternal _CMsgIPAddressBucket_default_instance_;
constexpr CMsgGCRoutingProtoBufHeader::CMsgGCRoutingProtoBufHeader(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : dst_gcid_queue_(PROTOBUF_ULONGLONG(0))
  , dst_gc_dir_index_(0u){}
struct CMsgGCRoutingProtoBufHeaderDefaultTypeInternal {
  constexpr CMsgGCRoutingProtoBufHeaderDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgGCRoutingProtoBufHeaderDefaultTypeInternal() {}
  union {
    CMsgGCRoutingProtoBufHeader _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgGCRoutingProtoBufHeaderDefaultTypeInternal _CMsgGCRoutingProtoBufHeader_default_instance_;
constexpr CMsgProtoBufHeader::CMsgProtoBufHeader(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : forward_to_sysid_()
  , exclude_client_sessionids_()
  , target_job_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , error_message_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , debug_source_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , wg_token_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , webui_auth_key_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , routing_gc_(nullptr)
  , steamid_(PROTOBUF_ULONGLONG(0))
  , client_sessionid_(0)
  , routing_appid_(0u)
  , auth_account_flags_(0u)
  , publisher_group_id_(0u)
  , sysid_(0u)
  , token_source_(0u)
  , seq_num_(0)
  , webapi_key_id_(0u)
  , cm_sysid_(0u)
  , launcher_type_(0u)
  , admin_spoofing_user_(false)
  , is_from_external_source_(false)
  , is_valveds_(false)
  , realm_(0u)
  , token_id_(PROTOBUF_ULONGLONG(0))
  , debug_source_string_index_(0u)
  , session_disposition_(0)

  , admin_request_spoofing_steamid_(PROTOBUF_ULONGLONG(0))
  , trace_tag_(PROTOBUF_ULONGLONG(0))
  , timeout_ms_(-1)
  , jobid_source_(PROTOBUF_ULONGLONG(18446744073709551615))
  , jobid_target_(PROTOBUF_ULONGLONG(18446744073709551615))
  , eresult_(2)
  , transport_error_(1)
  , messageid_(PROTOBUF_ULONGLONG(18446744073709551615))
  , _oneof_case_{}{}
struct CMsgProtoBufHeaderDefaultTypeInternal {
  constexpr CMsgProtoBufHeaderDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgProtoBufHeaderDefaultTypeInternal() {}
  union {
    CMsgProtoBufHeader _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgProtoBufHeaderDefaultTypeInternal _CMsgProtoBufHeader_default_instance_;
constexpr CMsgKubeRPCPacket_Hdr::CMsgKubeRPCPacket_Hdr(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : target_job_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , error_message_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , reply_address_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , eresult_(2)
  , jobid_source_(PROTOBUF_ULONGLONG(18446744073709551615))
  , jobid_target_(PROTOBUF_ULONGLONG(18446744073709551615)){}
struct CMsgKubeRPCPacket_HdrDefaultTypeInternal {
  constexpr CMsgKubeRPCPacket_HdrDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgKubeRPCPacket_HdrDefaultTypeInternal() {}
  union {
    CMsgKubeRPCPacket_Hdr _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgKubeRPCPacket_HdrDefaultTypeInternal _CMsgKubeRPCPacket_Hdr_default_instance_;
constexpr CMsgKubeRPCPacket::CMsgKubeRPCPacket(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : payload_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , hdr_(nullptr){}
struct CMsgKubeRPCPacketDefaultTypeInternal {
  constexpr CMsgKubeRPCPacketDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgKubeRPCPacketDefaultTypeInternal() {}
  union {
    CMsgKubeRPCPacket _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgKubeRPCPacketDefaultTypeInternal _CMsgKubeRPCPacket_default_instance_;
constexpr CMsgMulti::CMsgMulti(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : message_body_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , size_unzipped_(0u){}
struct CMsgMultiDefaultTypeInternal {
  constexpr CMsgMultiDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgMultiDefaultTypeInternal() {}
  union {
    CMsgMulti _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgMultiDefaultTypeInternal _CMsgMulti_default_instance_;
constexpr CMsgProtobufWrapped::CMsgProtobufWrapped(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : message_body_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string){}
struct CMsgProtobufWrappedDefaultTypeInternal {
  constexpr CMsgProtobufWrappedDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgProtobufWrappedDefaultTypeInternal() {}
  union {
    CMsgProtobufWrapped _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgProtobufWrappedDefaultTypeInternal _CMsgProtobufWrapped_default_instance_;
constexpr CMsgAuthTicket::CMsgAuthTicket(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : ticket_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , server_secret_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , steamid_(PROTOBUF_ULONGLONG(0))
  , estate_(0u)
  , h_steam_pipe_(0u)
  , gameid_(PROTOBUF_ULONGLONG(0))
  , ticket_crc_(0u)
  , ticket_type_(0u)
  , eresult_(2u){}
struct CMsgAuthTicketDefaultTypeInternal {
  constexpr CMsgAuthTicketDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgAuthTicketDefaultTypeInternal() {}
  union {
    CMsgAuthTicket _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgAuthTicketDefaultTypeInternal _CMsgAuthTicket_default_instance_;
constexpr CCDDBAppDetailCommon::CCDDBAppDetailCommon(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : content_descriptorids_()
  , content_descriptorids_including_dlc_()
  , name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , icon_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , friendly_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , propagation_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , appid_(0u)
  , tool_(false)
  , demo_(false)
  , media_(false)
  , community_visible_stats_(false)
  , app_type_(0u)
  , has_adult_content_(false)
  , is_visible_in_steam_china_(false)
  , has_adult_content_sex_(false)
  , has_adult_content_violence_(false){}
struct CCDDBAppDetailCommonDefaultTypeInternal {
  constexpr CCDDBAppDetailCommonDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CCDDBAppDetailCommonDefaultTypeInternal() {}
  union {
    CCDDBAppDetailCommon _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CCDDBAppDetailCommonDefaultTypeInternal _CCDDBAppDetailCommon_default_instance_;
constexpr CMsgAppRights::CMsgAppRights(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : edit_info_(false)
  , publish_(false)
  , view_error_data_(false)
  , download_(false)
  , upload_cdkeys_(false)
  , generate_cdkeys_(false)
  , view_financials_(false)
  , manage_ceg_(false)
  , manage_signing_(false)
  , manage_cdkeys_(false)
  , edit_marketing_(false)
  , economy_support_(false)
  , economy_support_supervisor_(false)
  , manage_pricing_(false)
  , broadcast_live_(false)
  , view_marketing_traffic_(false)
  , edit_store_display_content_(false){}
struct CMsgAppRightsDefaultTypeInternal {
  constexpr CMsgAppRightsDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgAppRightsDefaultTypeInternal() {}
  union {
    CMsgAppRights _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgAppRightsDefaultTypeInternal _CMsgAppRights_default_instance_;
constexpr CCuratorPreferences::CCuratorPreferences(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : tagids_curated_()
  , tagids_filtered_()
  , website_title_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , website_url_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , discussion_url_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , supported_languages_(0u)
  , platform_windows_(false)
  , platform_mac_(false)
  , platform_linux_(false)
  , vr_content_(false)
  , timestamp_updated_(0u)
  , adult_content_violence_(false)
  , adult_content_sex_(false)
  , show_broadcast_(false){}
struct CCuratorPreferencesDefaultTypeInternal {
  constexpr CCuratorPreferencesDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CCuratorPreferencesDefaultTypeInternal() {}
  union {
    CCuratorPreferences _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CCuratorPreferencesDefaultTypeInternal _CCuratorPreferences_default_instance_;
constexpr CLocalizationToken::CLocalizationToken(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : localized_string_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , language_(0u){}
struct CLocalizationTokenDefaultTypeInternal {
  constexpr CLocalizationTokenDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CLocalizationTokenDefaultTypeInternal() {}
  union {
    CLocalizationToken _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CLocalizationTokenDefaultTypeInternal _CLocalizationToken_default_instance_;
constexpr CClanEventUserNewsTuple::CClanEventUserNewsTuple(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : event_gid_(PROTOBUF_ULONGLONG(0))
  , clanid_(0u)
  , rtime_start_(0u)
  , announcement_gid_(PROTOBUF_ULONGLONG(0))
  , rtime_end_(0u)
  , priority_score_(0u)
  , type_(0u)
  , clamp_range_slot_(0u)
  , appid_(0u)
  , rtime32_last_modified_(0u){}
struct CClanEventUserNewsTupleDefaultTypeInternal {
  constexpr CClanEventUserNewsTupleDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CClanEventUserNewsTupleDefaultTypeInternal() {}
  union {
    CClanEventUserNewsTuple _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CClanEventUserNewsTupleDefaultTypeInternal _CClanEventUserNewsTuple_default_instance_;
constexpr CClanMatchEventByRange::CClanMatchEventByRange(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : events_()
  , rtime_before_(0u)
  , rtime_after_(0u)
  , qualified_(0u){}
struct CClanMatchEventByRangeDefaultTypeInternal {
  constexpr CClanMatchEventByRangeDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CClanMatchEventByRangeDefaultTypeInternal() {}
  union {
    CClanMatchEventByRange _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CClanMatchEventByRangeDefaultTypeInternal _CClanMatchEventByRange_default_instance_;
constexpr CCommunity_ClanAnnouncementInfo::CCommunity_ClanAnnouncementInfo(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : tags_()
  , headline_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , body_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , gid_(PROTOBUF_ULONGLONG(0))
  , clanid_(PROTOBUF_ULONGLONG(0))
  , posterid_(PROTOBUF_ULONGLONG(0))
  , posttime_(0u)
  , updatetime_(0u)
  , commentcount_(0)
  , language_(0)
  , forum_topic_id_(PROTOBUF_ULONGLONG(0))
  , event_gid_(PROTOBUF_ULONGLONG(0))
  , voteupcount_(0)
  , hidden_(false)
  , banned_(false)
  , votedowncount_(0)
  , ban_check_result_(0)
{}
struct CCommunity_ClanAnnouncementInfoDefaultTypeInternal {
  constexpr CCommunity_ClanAnnouncementInfoDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CCommunity_ClanAnnouncementInfoDefaultTypeInternal() {}
  union {
    CCommunity_ClanAnnouncementInfo _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CCommunity_ClanAnnouncementInfoDefaultTypeInternal _CCommunity_ClanAnnouncementInfo_default_instance_;
constexpr CClanEventData::CClanEventData(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : referenced_appids_()
  , event_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , server_address_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , server_password_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , event_notes_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , jsondata_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , build_branch_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , announcement_body_(nullptr)
  , gid_(PROTOBUF_ULONGLONG(0))
  , clan_steamid_(PROTOBUF_ULONGLONG(0))
  , appid_(0u)
  , rtime32_start_time_(0u)
  , rtime32_end_time_(0u)
  , comment_count_(0)
  , creator_steamid_(PROTOBUF_ULONGLONG(0))
  , last_update_steamid_(PROTOBUF_ULONGLONG(0))
  , rtime32_visibility_start_(0u)
  , rtime32_visibility_end_(0u)
  , broadcaster_accountid_(0u)
  , follower_count_(0u)
  , published_(false)
  , hidden_(false)
  , unlisted_(false)
  , ignore_count_(0u)
  , forum_topic_id_(PROTOBUF_ULONGLONG(0))
  , news_post_gid_(PROTOBUF_ULONGLONG(0))
  , rtime32_last_modified_(0u)
  , rtime_mod_reviewed_(0u)
  , featured_app_tagid_(0u)
  , build_id_(0u)
  , rtime_created_(0u)
  , event_type_(1)
{}
struct CClanEventDataDefaultTypeInternal {
  constexpr CClanEventDataDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CClanEventDataDefaultTypeInternal() {}
  union {
    CClanEventData _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CClanEventDataDefaultTypeInternal _CClanEventData_default_instance_;
constexpr CBilling_Address::CBilling_Address(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : first_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , last_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , address1_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , address2_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , city_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , us_state_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , country_code_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , postcode_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , phone_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , zip_plus4_(0){}
struct CBilling_AddressDefaultTypeInternal {
  constexpr CBilling_AddressDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CBilling_AddressDefaultTypeInternal() {}
  union {
    CBilling_Address _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CBilling_AddressDefaultTypeInternal _CBilling_Address_default_instance_;
constexpr CPackageReservationStatus::CPackageReservationStatus(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : reservation_country_code_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , notificaton_token_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , packageid_(0u)
  , reservation_state_(0)
  , queue_position_(0)
  , total_queue_size_(0)
  , expired_(false)
  , time_expires_(0u)
  , time_reserved_(0u)
  , rtime_estimated_notification_(0u)
  , queue_head_position_at_reservation_(0)
  , queue_head_position_now_(0){}
struct CPackageReservationStatusDefaultTypeInternal {
  constexpr CPackageReservationStatusDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CPackageReservationStatusDefaultTypeInternal() {}
  union {
    CPackageReservationStatus _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CPackageReservationStatusDefaultTypeInternal _CPackageReservationStatus_default_instance_;
constexpr CMsgKeyValuePair::CMsgKeyValuePair(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , value_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string){}
struct CMsgKeyValuePairDefaultTypeInternal {
  constexpr CMsgKeyValuePairDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgKeyValuePairDefaultTypeInternal() {}
  union {
    CMsgKeyValuePair _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgKeyValuePairDefaultTypeInternal _CMsgKeyValuePair_default_instance_;
constexpr CMsgKeyValueSet::CMsgKeyValueSet(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : pairs_(){}
struct CMsgKeyValueSetDefaultTypeInternal {
  constexpr CMsgKeyValueSetDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgKeyValueSetDefaultTypeInternal() {}
  union {
    CMsgKeyValueSet _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgKeyValueSetDefaultTypeInternal _CMsgKeyValueSet_default_instance_;
constexpr UserContentDescriptorPreferences_ContentDescriptor::UserContentDescriptorPreferences_ContentDescriptor(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : content_descriptorid_(0u)
  , timestamp_added_(0u){}
struct UserContentDescriptorPreferences_ContentDescriptorDefaultTypeInternal {
  constexpr UserContentDescriptorPreferences_ContentDescriptorDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~UserContentDescriptorPreferences_ContentDescriptorDefaultTypeInternal() {}
  union {
    UserContentDescriptorPreferences_ContentDescriptor _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT UserContentDescriptorPreferences_ContentDescriptorDefaultTypeInternal _UserContentDescriptorPreferences_ContentDescriptor_default_instance_;
constexpr UserContentDescriptorPreferences::UserContentDescriptorPreferences(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : content_descriptors_to_exclude_(){}
struct UserContentDescriptorPreferencesDefaultTypeInternal {
  constexpr UserContentDescriptorPreferencesDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~UserContentDescriptorPreferencesDefaultTypeInternal() {}
  union {
    UserContentDescriptorPreferences _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT UserContentDescriptorPreferencesDefaultTypeInternal _UserContentDescriptorPreferences_default_instance_;
constexpr UserSystemInformation::UserSystemInformation(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : manufacturer_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , model_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , dx_video_card_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , os_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , cpu_vendor_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , cpu_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , dx_driver_version_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , adapter_description_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , driver_version_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , driver_date_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , dx_vendorid_(0)
  , dx_deviceid_(0)
  , system_ram_(PROTOBUF_ULONGLONG(0))
  , num_gpu_(0u)
  , gaming_device_type_(0u)
  , vram_size_(0u)
  , screen_width_(0u)
  , screen_height_(0u)
  , precise_frame_rate_(false){}
struct UserSystemInformationDefaultTypeInternal {
  constexpr UserSystemInformationDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~UserSystemInformationDefaultTypeInternal() {}
  union {
    UserSystemInformation _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT UserSystemInformationDefaultTypeInternal _UserSystemInformation_default_instance_;
constexpr GamePerformanceSettings::GamePerformanceSettings(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : setting_(0)

  , game_resolution_width_(0u)
  , game_resolution_height_(0u){}
struct GamePerformanceSettingsDefaultTypeInternal {
  constexpr GamePerformanceSettingsDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~GamePerformanceSettingsDefaultTypeInternal() {}
  union {
    GamePerformanceSettings _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT GamePerformanceSettingsDefaultTypeInternal _GamePerformanceSettings_default_instance_;
bool CMsgProtoBufHeader_ESessionDisposition_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

static ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<std::string> CMsgProtoBufHeader_ESessionDisposition_strings[2] = {};

static const char CMsgProtoBufHeader_ESessionDisposition_names[] =
  "k_ESessionDispositionDisconnect"
  "k_ESessionDispositionNormal";

static const ::PROTOBUF_NAMESPACE_ID::internal::EnumEntry CMsgProtoBufHeader_ESessionDisposition_entries[] = {
  { {CMsgProtoBufHeader_ESessionDisposition_names + 0, 31}, 1 },
  { {CMsgProtoBufHeader_ESessionDisposition_names + 31, 27}, 0 },
};

static const int CMsgProtoBufHeader_ESessionDisposition_entries_by_number[] = {
  1,
  0,
};

const std::string& CMsgProtoBufHeader_ESessionDisposition_Name(
    CMsgProtoBufHeader_ESessionDisposition value) {
  static const bool dummy =
      ::PROTOBUF_NAMESPACE_ID::internal::InitializeEnumStrings(
          CMsgProtoBufHeader_ESessionDisposition_entries,
          CMsgProtoBufHeader_ESessionDisposition_entries_by_number,
          2, CMsgProtoBufHeader_ESessionDisposition_strings);
  (void) dummy;
  int idx = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumName(
      CMsgProtoBufHeader_ESessionDisposition_entries,
      CMsgProtoBufHeader_ESessionDisposition_entries_by_number,
      2, value);
  return idx == -1 ? ::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString() :
                     CMsgProtoBufHeader_ESessionDisposition_strings[idx].get();
}
bool CMsgProtoBufHeader_ESessionDisposition_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, CMsgProtoBufHeader_ESessionDisposition* value) {
  int int_value;
  bool success = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumValue(
      CMsgProtoBufHeader_ESessionDisposition_entries, 2, name, &int_value);
  if (success) {
    *value = static_cast<CMsgProtoBufHeader_ESessionDisposition>(int_value);
  }
  return success;
}
#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr CMsgProtoBufHeader_ESessionDisposition CMsgProtoBufHeader::k_ESessionDispositionNormal;
constexpr CMsgProtoBufHeader_ESessionDisposition CMsgProtoBufHeader::k_ESessionDispositionDisconnect;
constexpr CMsgProtoBufHeader_ESessionDisposition CMsgProtoBufHeader::ESessionDisposition_MIN;
constexpr CMsgProtoBufHeader_ESessionDisposition CMsgProtoBufHeader::ESessionDisposition_MAX;
constexpr int CMsgProtoBufHeader::ESessionDisposition_ARRAYSIZE;
#endif
bool GamePerformanceSettings_EGamePerformanceSetting_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

static ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<std::string> GamePerformanceSettings_EGamePerformanceSetting_strings[6] = {};

static const char GamePerformanceSettings_EGamePerformanceSetting_names[] =
  "k_EGamePerformanceSetting_Custom"
  "k_EGamePerformanceSetting_High"
  "k_EGamePerformanceSetting_Low"
  "k_EGamePerformanceSetting_Medium"
  "k_EGamePerformanceSetting_NotSet"
  "k_EGamePerformanceSetting_Ultra";

static const ::PROTOBUF_NAMESPACE_ID::internal::EnumEntry GamePerformanceSettings_EGamePerformanceSetting_entries[] = {
  { {GamePerformanceSettings_EGamePerformanceSetting_names + 0, 32}, 5 },
  { {GamePerformanceSettings_EGamePerformanceSetting_names + 32, 30}, 3 },
  { {GamePerformanceSettings_EGamePerformanceSetting_names + 62, 29}, 1 },
  { {GamePerformanceSettings_EGamePerformanceSetting_names + 91, 32}, 2 },
  { {GamePerformanceSettings_EGamePerformanceSetting_names + 123, 32}, 0 },
  { {GamePerformanceSettings_EGamePerformanceSetting_names + 155, 31}, 4 },
};

static const int GamePerformanceSettings_EGamePerformanceSetting_entries_by_number[] = {
  4,
  2,
  3,
  1,
  5,
  0,
};

const std::string& GamePerformanceSettings_EGamePerformanceSetting_Name(
    GamePerformanceSettings_EGamePerformanceSetting value) {
  static const bool dummy =
      ::PROTOBUF_NAMESPACE_ID::internal::InitializeEnumStrings(
          GamePerformanceSettings_EGamePerformanceSetting_entries,
          GamePerformanceSettings_EGamePerformanceSetting_entries_by_number,
          6, GamePerformanceSettings_EGamePerformanceSetting_strings);
  (void) dummy;
  int idx = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumName(
      GamePerformanceSettings_EGamePerformanceSetting_entries,
      GamePerformanceSettings_EGamePerformanceSetting_entries_by_number,
      6, value);
  return idx == -1 ? ::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString() :
                     GamePerformanceSettings_EGamePerformanceSetting_strings[idx].get();
}
bool GamePerformanceSettings_EGamePerformanceSetting_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, GamePerformanceSettings_EGamePerformanceSetting* value) {
  int int_value;
  bool success = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumValue(
      GamePerformanceSettings_EGamePerformanceSetting_entries, 6, name, &int_value);
  if (success) {
    *value = static_cast<GamePerformanceSettings_EGamePerformanceSetting>(int_value);
  }
  return success;
}
#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::k_EGamePerformanceSetting_NotSet;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::k_EGamePerformanceSetting_Low;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::k_EGamePerformanceSetting_Medium;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::k_EGamePerformanceSetting_High;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::k_EGamePerformanceSetting_Ultra;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::k_EGamePerformanceSetting_Custom;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::EGamePerformanceSetting_MIN;
constexpr GamePerformanceSettings_EGamePerformanceSetting GamePerformanceSettings::EGamePerformanceSetting_MAX;
constexpr int GamePerformanceSettings::EGamePerformanceSetting_ARRAYSIZE;
#endif
bool EBanContentCheckResult_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 5:
    case 30:
    case 50:
    case 75:
    case 100:
      return true;
    default:
      return false;
  }
}

static ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<std::string> EBanContentCheckResult_strings[8] = {};

static const char EBanContentCheckResult_names[] =
  "k_EBanContentCheckResult_Likely"
  "k_EBanContentCheckResult_NeedsChecking"
  "k_EBanContentCheckResult_NotScanned"
  "k_EBanContentCheckResult_Possible"
  "k_EBanContentCheckResult_Reset"
  "k_EBanContentCheckResult_Unlikely"
  "k_EBanContentCheckResult_VeryLikely"
  "k_EBanContentCheckResult_VeryUnlikely";

static const ::PROTOBUF_NAMESPACE_ID::internal::EnumEntry EBanContentCheckResult_entries[] = {
  { {EBanContentCheckResult_names + 0, 31}, 75 },
  { {EBanContentCheckResult_names + 31, 38}, 2 },
  { {EBanContentCheckResult_names + 69, 35}, 0 },
  { {EBanContentCheckResult_names + 104, 33}, 50 },
  { {EBanContentCheckResult_names + 137, 30}, 1 },
  { {EBanContentCheckResult_names + 167, 33}, 30 },
  { {EBanContentCheckResult_names + 200, 35}, 100 },
  { {EBanContentCheckResult_names + 235, 37}, 5 },
};

static const int EBanContentCheckResult_entries_by_number[] = {
  2,
  4,
  1,
  7,
  5,
  3,
  0,
  6,
};

const std::string& EBanContentCheckResult_Name(
    EBanContentCheckResult value) {
  static const bool dummy =
      ::PROTOBUF_NAMESPACE_ID::internal::InitializeEnumStrings(
          EBanContentCheckResult_entries,
          EBanContentCheckResult_entries_by_number,
          8, EBanContentCheckResult_strings);
  (void) dummy;
  int idx = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumName(
      EBanContentCheckResult_entries,
      EBanContentCheckResult_entries_by_number,
      8, value);
  return idx == -1 ? ::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString() :
                     EBanContentCheckResult_strings[idx].get();
}
bool EBanContentCheckResult_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, EBanContentCheckResult* value) {
  int int_value;
  bool success = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumValue(
      EBanContentCheckResult_entries, 8, name, &int_value);
  if (success) {
    *value = static_cast<EBanContentCheckResult>(int_value);
  }
  return success;
}
bool EProtoClanEventType_IsValid(int value) {
  switch (value) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
      return true;
    default:
      return false;
  }
}

static ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<std::string> EProtoClanEventType_strings[36] = {};

static const char EProtoClanEventType_names[] =
  "k_EClanBetaReleaseEvent"
  "k_EClanBroadcastEvent"
  "k_EClanChatEvent"
  "k_EClanCreatorHome"
  "k_EClanCrosspostEvent"
  "k_EClanDLCReleaseEvent"
  "k_EClanDevStreamEvent"
  "k_EClanESportTournamentStreamEvent"
  "k_EClanFamousStreamEvent"
  "k_EClanFreeTrial"
  "k_EClanFutureReleaseEvent"
  "k_EClanGameEvent"
  "k_EClanGameItemSalesEvent"
  "k_EClanGameReleaseEvent"
  "k_EClanGameSalesEvent"
  "k_EClanIRLEvent"
  "k_EClanInGameBonusXPEvent"
  "k_EClanInGameChallengeEvent"
  "k_EClanInGameContentReleaseEvent"
  "k_EClanInGameContestEvent"
  "k_EClanInGameEventGeneral"
  "k_EClanInGameLootEvent"
  "k_EClanInGamePerksEvent"
  "k_EClanMajorUpdateEvent"
  "k_EClanMeetingEvent"
  "k_EClanMusicAndArtsEvent"
  "k_EClanNewsEvent"
  "k_EClanOtherEvent"
  "k_EClanPartyEvent"
  "k_EClanPreAnnounceMajorUpdateEvent"
  "k_EClanSeasonRelease"
  "k_EClanSeasonUpdate"
  "k_EClanSmallUpdateEvent"
  "k_EClanSpecialCauseEvent"
  "k_EClanSportsEvent"
  "k_EClanTripEvent";

static const ::PROTOBUF_NAMESPACE_ID::internal::EnumEntry EProtoClanEventType_entries[] = {
  { {EProtoClanEventType_names + 0, 23}, 29 },
  { {EProtoClanEventType_names + 23, 21}, 11 },
  { {EProtoClanEventType_names + 44, 16}, 9 },
  { {EProtoClanEventType_names + 60, 18}, 36 },
  { {EProtoClanEventType_names + 78, 21}, 34 },
  { {EProtoClanEventType_names + 99, 22}, 15 },
  { {EProtoClanEventType_names + 121, 21}, 18 },
  { {EProtoClanEventType_names + 142, 34}, 17 },
  { {EProtoClanEventType_names + 176, 24}, 19 },
  { {EProtoClanEventType_names + 200, 16}, 31 },
  { {EProtoClanEventType_names + 216, 25}, 16 },
  { {EProtoClanEventType_names + 241, 16}, 2 },
  { {EProtoClanEventType_names + 257, 25}, 21 },
  { {EProtoClanEventType_names + 282, 23}, 10 },
  { {EProtoClanEventType_names + 305, 21}, 20 },
  { {EProtoClanEventType_names + 326, 15}, 27 },
  { {EProtoClanEventType_names + 341, 25}, 22 },
  { {EProtoClanEventType_names + 366, 27}, 25 },
  { {EProtoClanEventType_names + 393, 32}, 30 },
  { {EProtoClanEventType_names + 425, 25}, 26 },
  { {EProtoClanEventType_names + 450, 25}, 35 },
  { {EProtoClanEventType_names + 475, 22}, 23 },
  { {EProtoClanEventType_names + 497, 23}, 24 },
  { {EProtoClanEventType_names + 520, 23}, 14 },
  { {EProtoClanEventType_names + 543, 19}, 4 },
  { {EProtoClanEventType_names + 562, 24}, 6 },
  { {EProtoClanEventType_names + 586, 16}, 28 },
  { {EProtoClanEventType_names + 602, 17}, 1 },
  { {EProtoClanEventType_names + 619, 17}, 3 },
  { {EProtoClanEventType_names + 636, 34}, 13 },
  { {EProtoClanEventType_names + 670, 20}, 32 },
  { {EProtoClanEventType_names + 690, 19}, 33 },
  { {EProtoClanEventType_names + 709, 23}, 12 },
  { {EProtoClanEventType_names + 732, 24}, 5 },
  { {EProtoClanEventType_names + 756, 18}, 7 },
  { {EProtoClanEventType_names + 774, 16}, 8 },
};

static const int EProtoClanEventType_entries_by_number[] = {
  27,
  11,
  28,
  24,
  33,
  25,
  34,
  35,
  2,
  13,
  1,
  32,
  29,
  23,
  5,
  10,
  7,
  6,
  8,
  14,
  12,
  16,
  21,
  22,
  17,
  19,
  15,
  26,
  0,
  18,
  9,
  30,
  31,
  4,
  20,
  3,
};

const std::string& EProtoClanEventType_Name(
    EProtoClanEventType value) {
  static const bool dummy =
      ::PROTOBUF_NAMESPACE_ID::internal::InitializeEnumStrings(
          EProtoClanEventType_entries,
          EProtoClanEventType_entries_by_number,
          36, EProtoClanEventType_strings);
  (void) dummy;
  int idx = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumName(
      EProtoClanEventType_entries,
      EProtoClanEventType_entries_by_number,
      36, value);
  return idx == -1 ? ::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString() :
                     EProtoClanEventType_strings[idx].get();
}
bool EProtoClanEventType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, EProtoClanEventType* value) {
  int int_value;
  bool success = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumValue(
      EProtoClanEventType_entries, 36, name, &int_value);
  if (success) {
    *value = static_cast<EProtoClanEventType>(int_value);
  }
  return success;
}
bool PartnerEventNotificationType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

static ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<std::string> PartnerEventNotificationType_strings[4] = {};

static const char PartnerEventNotificationType_names[] =
  "k_EEventBroadcastStart"
  "k_EEventMatchStart"
  "k_EEventPartnerMaxType"
  "k_EEventStart";

static const ::PROTOBUF_NAMESPACE_ID::internal::EnumEntry PartnerEventNotificationType_entries[] = {
  { {PartnerEventNotificationType_names + 0, 22}, 1 },
  { {PartnerEventNotificationType_names + 22, 18}, 2 },
  { {PartnerEventNotificationType_names + 40, 22}, 3 },
  { {PartnerEventNotificationType_names + 62, 13}, 0 },
};

static const int PartnerEventNotificationType_entries_by_number[] = {
  3,
  0,
  1,
  2,
};

const std::string& PartnerEventNotificationType_Name(
    PartnerEventNotificationType value) {
  static const bool dummy =
      ::PROTOBUF_NAMESPACE_ID::internal::InitializeEnumStrings(
          PartnerEventNotificationType_entries,
          PartnerEventNotificationType_entries_by_number,
          4, PartnerEventNotificationType_strings);
  (void) dummy;
  int idx = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumName(
      PartnerEventNotificationType_entries,
      PartnerEventNotificationType_entries_by_number,
      4, value);
  return idx == -1 ? ::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString() :
                     PartnerEventNotificationType_strings[idx].get();
}
bool PartnerEventNotificationType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, PartnerEventNotificationType* value) {
  int int_value;
  bool success = ::PROTOBUF_NAMESPACE_ID::internal::LookUpEnumValue(
      PartnerEventNotificationType_entries, 4, name, &int_value);
  if (success) {
    *value = static_cast<PartnerEventNotificationType>(int_value);
  }
  return success;
}

class CMsgIPAddress::_Internal {
 public:
};

CMsgIPAddress::CMsgIPAddress(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgIPAddress::CMsgIPAddress(const CMsgIPAddress& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite() {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  clear_has_ip();
  switch (from.ip_case()) {
    case kV4: {
      _internal_set_v4(from._internal_v4());
      break;
    }
    case kV6: {
      _internal_set_v6(from._internal_v6());
      break;
    }
    case IP_NOT_SET: {
      break;
    }
  }

}

void CMsgIPAddress::SharedCtor() {
clear_has_ip();
}

CMsgIPAddress::~CMsgIPAddress() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgIPAddress::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  if (has_ip()) {
    clear_ip();
  }
}

void CMsgIPAddress::ArenaDtor(void* object) {
  CMsgIPAddress* _this = reinterpret_cast< CMsgIPAddress* >(object);
  (void)_this;
}
void CMsgIPAddress::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgIPAddress::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgIPAddress::clear_ip() {

  switch (ip_case()) {
    case kV4: {

      break;
    }
    case kV6: {
      ip_.v6_.Destroy(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
      break;
    }
    case IP_NOT_SET: {
      break;
    }
  }
  _oneof_case_[0] = IP_NOT_SET;
}

void CMsgIPAddress::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  clear_ip();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgIPAddress::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 13)) {
          _internal_set_v4(::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint32>(ptr));
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint32);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_v6();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgIPAddress::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  switch (ip_case()) {
    case kV4: {
      target = stream->EnsureSpace(target);
      target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed32ToArray(1, this->_internal_v4(), target);
      break;
    }
    case kV6: {
      target = stream->WriteBytesMaybeAliased(
          2, this->_internal_v6(), target);
      break;
    }
    default: ;
  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgIPAddress::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  switch (ip_case()) {

    case kV4: {
      total_size += 1 + 4;
      break;
    }

    case kV6: {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_v6());
      break;
    }
    case IP_NOT_SET: {
      break;
    }
  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgIPAddress::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgIPAddress*>(
      &from));
}

void CMsgIPAddress::MergeFrom(const CMsgIPAddress& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  switch (from.ip_case()) {
    case kV4: {
      _internal_set_v4(from._internal_v4());
      break;
    }
    case kV6: {
      _internal_set_v6(from._internal_v6());
      break;
    }
    case IP_NOT_SET: {
      break;
    }
  }
}

void CMsgIPAddress::CopyFrom(const CMsgIPAddress& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgIPAddress::IsInitialized() const {
  return true;
}

void CMsgIPAddress::InternalSwap(CMsgIPAddress* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(ip_, other->ip_);
  swap(_oneof_case_[0], other->_oneof_case_[0]);
}

std::string CMsgIPAddress::GetTypeName() const {
  return "CMsgIPAddress";
}

class CMsgIPAddressBucket::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgIPAddressBucket>()._has_bits_);
  static const ::CMsgIPAddress& original_ip_address(const CMsgIPAddressBucket* msg);
  static void set_has_original_ip_address(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_bucket(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

const ::CMsgIPAddress&
CMsgIPAddressBucket::_Internal::original_ip_address(const CMsgIPAddressBucket* msg) {
  return *msg->original_ip_address_;
}
CMsgIPAddressBucket::CMsgIPAddressBucket(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgIPAddressBucket::CMsgIPAddressBucket(const CMsgIPAddressBucket& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  if (from._internal_has_original_ip_address()) {
    original_ip_address_ = new ::CMsgIPAddress(*from.original_ip_address_);
  } else {
    original_ip_address_ = nullptr;
  }
  bucket_ = from.bucket_;

}

void CMsgIPAddressBucket::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&original_ip_address_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&bucket_) -
    reinterpret_cast<char*>(&original_ip_address_)) + sizeof(bucket_));
}

CMsgIPAddressBucket::~CMsgIPAddressBucket() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgIPAddressBucket::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  if (this != internal_default_instance()) delete original_ip_address_;
}

void CMsgIPAddressBucket::ArenaDtor(void* object) {
  CMsgIPAddressBucket* _this = reinterpret_cast< CMsgIPAddressBucket* >(object);
  (void)_this;
}
void CMsgIPAddressBucket::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgIPAddressBucket::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgIPAddressBucket::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    GOOGLE_DCHECK(original_ip_address_ != nullptr);
    original_ip_address_->Clear();
  }
  bucket_ = PROTOBUF_ULONGLONG(0);
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgIPAddressBucket::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_original_ip_address(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 17)) {
          _Internal::set_has_bucket(&has_bits);
          bucket_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgIPAddressBucket::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        1, _Internal::original_ip_address(this), target, stream);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(2, this->_internal_bucket(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgIPAddressBucket::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *original_ip_address_);
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 + 8;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgIPAddressBucket::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgIPAddressBucket*>(
      &from));
}

void CMsgIPAddressBucket::MergeFrom(const CMsgIPAddressBucket& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_mutable_original_ip_address()->::CMsgIPAddress::MergeFrom(from._internal_original_ip_address());
    }
    if (cached_has_bits & 0x00000002u) {
      bucket_ = from.bucket_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgIPAddressBucket::CopyFrom(const CMsgIPAddressBucket& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgIPAddressBucket::IsInitialized() const {
  return true;
}

void CMsgIPAddressBucket::InternalSwap(CMsgIPAddressBucket* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgIPAddressBucket, bucket_)
      + sizeof(CMsgIPAddressBucket::bucket_)
      - PROTOBUF_FIELD_OFFSET(CMsgIPAddressBucket, original_ip_address_)>(
          reinterpret_cast<char*>(&original_ip_address_),
          reinterpret_cast<char*>(&other->original_ip_address_));
}

std::string CMsgIPAddressBucket::GetTypeName() const {
  return "CMsgIPAddressBucket";
}

class CMsgGCRoutingProtoBufHeader::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgGCRoutingProtoBufHeader>()._has_bits_);
  static void set_has_dst_gcid_queue(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_dst_gc_dir_index(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

CMsgGCRoutingProtoBufHeader::CMsgGCRoutingProtoBufHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgGCRoutingProtoBufHeader::CMsgGCRoutingProtoBufHeader(const CMsgGCRoutingProtoBufHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&dst_gcid_queue_, &from.dst_gcid_queue_,
    static_cast<size_t>(reinterpret_cast<char*>(&dst_gc_dir_index_) -
    reinterpret_cast<char*>(&dst_gcid_queue_)) + sizeof(dst_gc_dir_index_));

}

void CMsgGCRoutingProtoBufHeader::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&dst_gcid_queue_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&dst_gc_dir_index_) -
    reinterpret_cast<char*>(&dst_gcid_queue_)) + sizeof(dst_gc_dir_index_));
}

CMsgGCRoutingProtoBufHeader::~CMsgGCRoutingProtoBufHeader() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgGCRoutingProtoBufHeader::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgGCRoutingProtoBufHeader::ArenaDtor(void* object) {
  CMsgGCRoutingProtoBufHeader* _this = reinterpret_cast< CMsgGCRoutingProtoBufHeader* >(object);
  (void)_this;
}
void CMsgGCRoutingProtoBufHeader::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgGCRoutingProtoBufHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgGCRoutingProtoBufHeader::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    ::memset(&dst_gcid_queue_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&dst_gc_dir_index_) -
        reinterpret_cast<char*>(&dst_gcid_queue_)) + sizeof(dst_gc_dir_index_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgGCRoutingProtoBufHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_dst_gcid_queue(&has_bits);
          dst_gcid_queue_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_dst_gc_dir_index(&has_bits);
          dst_gc_dir_index_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgGCRoutingProtoBufHeader::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(1, this->_internal_dst_gcid_queue(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_dst_gc_dir_index(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgGCRoutingProtoBufHeader::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_dst_gcid_queue());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_dst_gc_dir_index());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgGCRoutingProtoBufHeader::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgGCRoutingProtoBufHeader*>(
      &from));
}

void CMsgGCRoutingProtoBufHeader::MergeFrom(const CMsgGCRoutingProtoBufHeader& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      dst_gcid_queue_ = from.dst_gcid_queue_;
    }
    if (cached_has_bits & 0x00000002u) {
      dst_gc_dir_index_ = from.dst_gc_dir_index_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgGCRoutingProtoBufHeader::CopyFrom(const CMsgGCRoutingProtoBufHeader& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgGCRoutingProtoBufHeader::IsInitialized() const {
  return true;
}

void CMsgGCRoutingProtoBufHeader::InternalSwap(CMsgGCRoutingProtoBufHeader* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgGCRoutingProtoBufHeader, dst_gc_dir_index_)
      + sizeof(CMsgGCRoutingProtoBufHeader::dst_gc_dir_index_)
      - PROTOBUF_FIELD_OFFSET(CMsgGCRoutingProtoBufHeader, dst_gcid_queue_)>(
          reinterpret_cast<char*>(&dst_gcid_queue_),
          reinterpret_cast<char*>(&other->dst_gcid_queue_));
}

std::string CMsgGCRoutingProtoBufHeader::GetTypeName() const {
  return "CMsgGCRoutingProtoBufHeader";
}

class CMsgProtoBufHeader::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgProtoBufHeader>()._has_bits_);
  static void set_has_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_client_sessionid(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_routing_appid(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_jobid_source(HasBits* has_bits) {
    (*has_bits)[0] |= 134217728u;
  }
  static void set_has_jobid_target(HasBits* has_bits) {
    (*has_bits)[0] |= 268435456u;
  }
  static void set_has_target_job_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_seq_num(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_eresult(HasBits* has_bits) {
    (*has_bits)[0] |= 536870912u;
  }
  static void set_has_error_message(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_auth_account_flags(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_token_source(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_admin_spoofing_user(HasBits* has_bits) {
    (*has_bits)[0] |= 131072u;
  }
  static void set_has_transport_error(HasBits* has_bits) {
    (*has_bits)[0] |= 1073741824u;
  }
  static void set_has_messageid(HasBits* has_bits) {
    (*has_bits)[0] |= 2147483648u;
  }
  static void set_has_publisher_group_id(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_sysid(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_webapi_key_id(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_is_from_external_source(HasBits* has_bits) {
    (*has_bits)[0] |= 262144u;
  }
  static void set_has_cm_sysid(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_launcher_type(HasBits* has_bits) {
    (*has_bits)[0] |= 65536u;
  }
  static void set_has_realm(HasBits* has_bits) {
    (*has_bits)[0] |= 1048576u;
  }
  static void set_has_timeout_ms(HasBits* has_bits) {
    (*has_bits)[0] |= 67108864u;
  }
  static void set_has_debug_source(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_debug_source_string_index(HasBits* has_bits) {
    (*has_bits)[0] |= 4194304u;
  }
  static void set_has_token_id(HasBits* has_bits) {
    (*has_bits)[0] |= 2097152u;
  }
  static const ::CMsgGCRoutingProtoBufHeader& routing_gc(const CMsgProtoBufHeader* msg);
  static void set_has_routing_gc(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_session_disposition(HasBits* has_bits) {
    (*has_bits)[0] |= 8388608u;
  }
  static void set_has_wg_token(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_webui_auth_key(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_admin_request_spoofing_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 16777216u;
  }
  static void set_has_is_valveds(HasBits* has_bits) {
    (*has_bits)[0] |= 524288u;
  }
  static void set_has_trace_tag(HasBits* has_bits) {
    (*has_bits)[0] |= 33554432u;
  }
};

const ::CMsgGCRoutingProtoBufHeader&
CMsgProtoBufHeader::_Internal::routing_gc(const CMsgProtoBufHeader* msg) {
  return *msg->routing_gc_;
}
CMsgProtoBufHeader::CMsgProtoBufHeader(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  forward_to_sysid_(arena),
  exclude_client_sessionids_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgProtoBufHeader::CMsgProtoBufHeader(const CMsgProtoBufHeader& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_),
      forward_to_sysid_(from.forward_to_sysid_),
      exclude_client_sessionids_(from.exclude_client_sessionids_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  target_job_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_target_job_name()) {
    target_job_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_target_job_name(),
      GetArena());
  }
  error_message_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_error_message()) {
    error_message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_error_message(),
      GetArena());
  }
  debug_source_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_debug_source()) {
    debug_source_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_debug_source(),
      GetArena());
  }
  wg_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_wg_token()) {
    wg_token_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_wg_token(),
      GetArena());
  }
  webui_auth_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_webui_auth_key()) {
    webui_auth_key_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_webui_auth_key(),
      GetArena());
  }
  if (from._internal_has_routing_gc()) {
    routing_gc_ = new ::CMsgGCRoutingProtoBufHeader(*from.routing_gc_);
  } else {
    routing_gc_ = nullptr;
  }
  ::memcpy(&steamid_, &from.steamid_,
    static_cast<size_t>(reinterpret_cast<char*>(&messageid_) -
    reinterpret_cast<char*>(&steamid_)) + sizeof(messageid_));
  clear_has_ip_addr();
  switch (from.ip_addr_case()) {
    case kIp: {
      _internal_set_ip(from._internal_ip());
      break;
    }
    case kIpV6: {
      _internal_set_ip_v6(from._internal_ip_v6());
      break;
    }
    case IP_ADDR_NOT_SET: {
      break;
    }
  }

}

void CMsgProtoBufHeader::SharedCtor() {
target_job_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
error_message_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
debug_source_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
wg_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
webui_auth_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&routing_gc_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&trace_tag_) -
    reinterpret_cast<char*>(&routing_gc_)) + sizeof(trace_tag_));
timeout_ms_ = -1;
jobid_source_ = PROTOBUF_ULONGLONG(18446744073709551615);
jobid_target_ = PROTOBUF_ULONGLONG(18446744073709551615);
eresult_ = 2;
transport_error_ = 1;
messageid_ = PROTOBUF_ULONGLONG(18446744073709551615);
clear_has_ip_addr();
}

CMsgProtoBufHeader::~CMsgProtoBufHeader() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgProtoBufHeader::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  target_job_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  error_message_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  debug_source_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  wg_token_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  webui_auth_key_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete routing_gc_;
  if (has_ip_addr()) {
    clear_ip_addr();
  }
}

void CMsgProtoBufHeader::ArenaDtor(void* object) {
  CMsgProtoBufHeader* _this = reinterpret_cast< CMsgProtoBufHeader* >(object);
  (void)_this;
}
void CMsgProtoBufHeader::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgProtoBufHeader::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgProtoBufHeader::clear_ip_addr() {

  switch (ip_addr_case()) {
    case kIp: {

      break;
    }
    case kIpV6: {
      ip_addr_.ip_v6_.Destroy(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
      break;
    }
    case IP_ADDR_NOT_SET: {
      break;
    }
  }
  _oneof_case_[0] = IP_ADDR_NOT_SET;
}

void CMsgProtoBufHeader::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  forward_to_sysid_.Clear();
  exclude_client_sessionids_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000003fu) {
    if (cached_has_bits & 0x00000001u) {
      target_job_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      error_message_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      debug_source_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      wg_token_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000010u) {
      webui_auth_key_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000020u) {
      GOOGLE_DCHECK(routing_gc_ != nullptr);
      routing_gc_->Clear();
    }
  }
  if (cached_has_bits & 0x000000c0u) {
    ::memset(&steamid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&client_sessionid_) -
        reinterpret_cast<char*>(&steamid_)) + sizeof(client_sessionid_));
  }
  if (cached_has_bits & 0x0000ff00u) {
    ::memset(&routing_appid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&cm_sysid_) -
        reinterpret_cast<char*>(&routing_appid_)) + sizeof(cm_sysid_));
  }
  if (cached_has_bits & 0x00ff0000u) {
    ::memset(&launcher_type_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&session_disposition_) -
        reinterpret_cast<char*>(&launcher_type_)) + sizeof(session_disposition_));
  }
  if (cached_has_bits & 0xff000000u) {
    ::memset(&admin_request_spoofing_steamid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&trace_tag_) -
        reinterpret_cast<char*>(&admin_request_spoofing_steamid_)) + sizeof(trace_tag_));
    timeout_ms_ = -1;
    jobid_source_ = PROTOBUF_ULONGLONG(18446744073709551615);
    jobid_target_ = PROTOBUF_ULONGLONG(18446744073709551615);
    eresult_ = 2;
    transport_error_ = 1;
    messageid_ = PROTOBUF_ULONGLONG(18446744073709551615);
  }
  clear_ip_addr();
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgProtoBufHeader::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 9)) {
          _Internal::set_has_steamid(&has_bits);
          steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_client_sessionid(&has_bits);
          client_sessionid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_routing_appid(&has_bits);
          routing_appid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 81)) {
          _Internal::set_has_jobid_source(&has_bits);
          jobid_source_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 89)) {
          _Internal::set_has_jobid_target(&has_bits);
          jobid_target_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 98)) {
          auto str = _internal_mutable_target_job_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 104)) {
          _Internal::set_has_eresult(&has_bits);
          eresult_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 114)) {
          auto str = _internal_mutable_error_message();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 120)) {
          _internal_set_ip(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 128)) {
          _Internal::set_has_auth_account_flags(&has_bits);
          auth_account_flags_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_transport_error(&has_bits);
          transport_error_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 18:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          _Internal::set_has_messageid(&has_bits);
          messageid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 19:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 152)) {
          _Internal::set_has_publisher_group_id(&has_bits);
          publisher_group_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 160)) {
          _Internal::set_has_sysid(&has_bits);
          sysid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 22:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 176)) {
          _Internal::set_has_token_source(&has_bits);
          token_source_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 23:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 184)) {
          _Internal::set_has_admin_spoofing_user(&has_bits);
          admin_spoofing_user_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 24:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 192)) {
          _Internal::set_has_seq_num(&has_bits);
          seq_num_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 25:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 200)) {
          _Internal::set_has_webapi_key_id(&has_bits);
          webapi_key_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 26:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 208)) {
          _Internal::set_has_is_from_external_source(&has_bits);
          is_from_external_source_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 27:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 216)) {
          ptr -= 2;
          do {
            ptr += 2;
            _internal_add_forward_to_sysid(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<216>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 218) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedUInt32Parser(_internal_mutable_forward_to_sysid(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 28:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 224)) {
          _Internal::set_has_cm_sysid(&has_bits);
          cm_sysid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 29:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 234)) {
          auto str = _internal_mutable_ip_v6();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 31:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 248)) {
          _Internal::set_has_launcher_type(&has_bits);
          launcher_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 32:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 0)) {
          _Internal::set_has_realm(&has_bits);
          realm_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 33:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_timeout_ms(&has_bits);
          timeout_ms_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 34:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_debug_source();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 35:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_debug_source_string_index(&has_bits);
          debug_source_string_index_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 36:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_token_id(&has_bits);
          token_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 37:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          ptr = ctx->ParseMessage(_internal_mutable_routing_gc(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 38:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::CMsgProtoBufHeader_ESessionDisposition_IsValid(val))) {
            _internal_set_session_disposition(static_cast<::CMsgProtoBufHeader_ESessionDisposition>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(38, val, mutable_unknown_fields());
          }
        } else goto handle_unusual;
        continue;

      case 39:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_wg_token();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 40:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 66)) {
          auto str = _internal_mutable_webui_auth_key();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 41:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          ptr -= 2;
          do {
            ptr += 2;
            _internal_add_exclude_client_sessionids(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<328>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedInt32Parser(_internal_mutable_exclude_client_sessionids(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 43:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 89)) {
          _Internal::set_has_admin_request_spoofing_steamid(&has_bits);
          admin_request_spoofing_steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 44:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 96)) {
          _Internal::set_has_is_valveds(&has_bits);
          is_valveds_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 45:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 105)) {
          _Internal::set_has_trace_tag(&has_bits);
          trace_tag_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgProtoBufHeader::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(1, this->_internal_steamid(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_client_sessionid(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_routing_appid(), target);
  }

  if (cached_has_bits & 0x08000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(10, this->_internal_jobid_source(), target);
  }

  if (cached_has_bits & 0x10000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(11, this->_internal_jobid_target(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        12, this->_internal_target_job_name(), target);
  }

  if (cached_has_bits & 0x20000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(13, this->_internal_eresult(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        14, this->_internal_error_message(), target);
  }

  if (_internal_has_ip()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(15, this->_internal_ip(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(16, this->_internal_auth_account_flags(), target);
  }

  if (cached_has_bits & 0x40000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(17, this->_internal_transport_error(), target);
  }

  if (cached_has_bits & 0x80000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(18, this->_internal_messageid(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(19, this->_internal_publisher_group_id(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(20, this->_internal_sysid(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(22, this->_internal_token_source(), target);
  }

  if (cached_has_bits & 0x00020000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(23, this->_internal_admin_spoofing_user(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(24, this->_internal_seq_num(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(25, this->_internal_webapi_key_id(), target);
  }

  if (cached_has_bits & 0x00040000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(26, this->_internal_is_from_external_source(), target);
  }

  for (int i = 0, n = this->_internal_forward_to_sysid_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(27, this->_internal_forward_to_sysid(i), target);
  }

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(28, this->_internal_cm_sysid(), target);
  }

  if (_internal_has_ip_v6()) {
    target = stream->WriteBytesMaybeAliased(
        29, this->_internal_ip_v6(), target);
  }

  if (cached_has_bits & 0x00010000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(31, this->_internal_launcher_type(), target);
  }

  if (cached_has_bits & 0x00100000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(32, this->_internal_realm(), target);
  }

  if (cached_has_bits & 0x04000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(33, this->_internal_timeout_ms(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        34, this->_internal_debug_source(), target);
  }

  if (cached_has_bits & 0x00400000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(35, this->_internal_debug_source_string_index(), target);
  }

  if (cached_has_bits & 0x00200000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(36, this->_internal_token_id(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        37, _Internal::routing_gc(this), target, stream);
  }

  if (cached_has_bits & 0x00800000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      38, this->_internal_session_disposition(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteStringMaybeAliased(
        39, this->_internal_wg_token(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->WriteStringMaybeAliased(
        40, this->_internal_webui_auth_key(), target);
  }

  for (int i = 0, n = this->_internal_exclude_client_sessionids_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(41, this->_internal_exclude_client_sessionids(i), target);
  }

  if (cached_has_bits & 0x01000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(43, this->_internal_admin_request_spoofing_steamid(), target);
  }

  if (cached_has_bits & 0x00080000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(44, this->_internal_is_valveds(), target);
  }

  if (cached_has_bits & 0x02000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(45, this->_internal_trace_tag(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgProtoBufHeader::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      UInt32Size(this->forward_to_sysid_);
    total_size += 2 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_forward_to_sysid_size());
    total_size += data_size;
  }

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      Int32Size(this->exclude_client_sessionids_);
    total_size += 2 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_exclude_client_sessionids_size());
    total_size += data_size;
  }

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_target_job_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_error_message());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_debug_source());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_wg_token());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_webui_auth_key());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *routing_gc_);
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_client_sessionid());
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_routing_appid());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_auth_account_flags());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_publisher_group_id());
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_sysid());
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_token_source());
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_seq_num());
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_webapi_key_id());
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_cm_sysid());
    }

  }
  if (cached_has_bits & 0x00ff0000u) {

    if (cached_has_bits & 0x00010000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_launcher_type());
    }

    if (cached_has_bits & 0x00020000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00040000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00080000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00100000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_realm());
    }

    if (cached_has_bits & 0x00200000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_token_id());
    }

    if (cached_has_bits & 0x00400000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_debug_source_string_index());
    }

    if (cached_has_bits & 0x00800000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_session_disposition());
    }

  }
  if (cached_has_bits & 0xff000000u) {

    if (cached_has_bits & 0x01000000u) {
      total_size += 2 + 8;
    }

    if (cached_has_bits & 0x02000000u) {
      total_size += 2 + 8;
    }

    if (cached_has_bits & 0x04000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_timeout_ms());
    }

    if (cached_has_bits & 0x08000000u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x10000000u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x20000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_eresult());
    }

    if (cached_has_bits & 0x40000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_transport_error());
    }

    if (cached_has_bits & 0x80000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_messageid());
    }

  }
  switch (ip_addr_case()) {

    case kIp: {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_ip());
      break;
    }

    case kIpV6: {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_ip_v6());
      break;
    }
    case IP_ADDR_NOT_SET: {
      break;
    }
  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgProtoBufHeader::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgProtoBufHeader*>(
      &from));
}

void CMsgProtoBufHeader::MergeFrom(const CMsgProtoBufHeader& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  forward_to_sysid_.MergeFrom(from.forward_to_sysid_);
  exclude_client_sessionids_.MergeFrom(from.exclude_client_sessionids_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_target_job_name(from._internal_target_job_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_error_message(from._internal_error_message());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_debug_source(from._internal_debug_source());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_wg_token(from._internal_wg_token());
    }
    if (cached_has_bits & 0x00000010u) {
      _internal_set_webui_auth_key(from._internal_webui_auth_key());
    }
    if (cached_has_bits & 0x00000020u) {
      _internal_mutable_routing_gc()->::CMsgGCRoutingProtoBufHeader::MergeFrom(from._internal_routing_gc());
    }
    if (cached_has_bits & 0x00000040u) {
      steamid_ = from.steamid_;
    }
    if (cached_has_bits & 0x00000080u) {
      client_sessionid_ = from.client_sessionid_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      routing_appid_ = from.routing_appid_;
    }
    if (cached_has_bits & 0x00000200u) {
      auth_account_flags_ = from.auth_account_flags_;
    }
    if (cached_has_bits & 0x00000400u) {
      publisher_group_id_ = from.publisher_group_id_;
    }
    if (cached_has_bits & 0x00000800u) {
      sysid_ = from.sysid_;
    }
    if (cached_has_bits & 0x00001000u) {
      token_source_ = from.token_source_;
    }
    if (cached_has_bits & 0x00002000u) {
      seq_num_ = from.seq_num_;
    }
    if (cached_has_bits & 0x00004000u) {
      webapi_key_id_ = from.webapi_key_id_;
    }
    if (cached_has_bits & 0x00008000u) {
      cm_sysid_ = from.cm_sysid_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00ff0000u) {
    if (cached_has_bits & 0x00010000u) {
      launcher_type_ = from.launcher_type_;
    }
    if (cached_has_bits & 0x00020000u) {
      admin_spoofing_user_ = from.admin_spoofing_user_;
    }
    if (cached_has_bits & 0x00040000u) {
      is_from_external_source_ = from.is_from_external_source_;
    }
    if (cached_has_bits & 0x00080000u) {
      is_valveds_ = from.is_valveds_;
    }
    if (cached_has_bits & 0x00100000u) {
      realm_ = from.realm_;
    }
    if (cached_has_bits & 0x00200000u) {
      token_id_ = from.token_id_;
    }
    if (cached_has_bits & 0x00400000u) {
      debug_source_string_index_ = from.debug_source_string_index_;
    }
    if (cached_has_bits & 0x00800000u) {
      session_disposition_ = from.session_disposition_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0xff000000u) {
    if (cached_has_bits & 0x01000000u) {
      admin_request_spoofing_steamid_ = from.admin_request_spoofing_steamid_;
    }
    if (cached_has_bits & 0x02000000u) {
      trace_tag_ = from.trace_tag_;
    }
    if (cached_has_bits & 0x04000000u) {
      timeout_ms_ = from.timeout_ms_;
    }
    if (cached_has_bits & 0x08000000u) {
      jobid_source_ = from.jobid_source_;
    }
    if (cached_has_bits & 0x10000000u) {
      jobid_target_ = from.jobid_target_;
    }
    if (cached_has_bits & 0x20000000u) {
      eresult_ = from.eresult_;
    }
    if (cached_has_bits & 0x40000000u) {
      transport_error_ = from.transport_error_;
    }
    if (cached_has_bits & 0x80000000u) {
      messageid_ = from.messageid_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  switch (from.ip_addr_case()) {
    case kIp: {
      _internal_set_ip(from._internal_ip());
      break;
    }
    case kIpV6: {
      _internal_set_ip_v6(from._internal_ip_v6());
      break;
    }
    case IP_ADDR_NOT_SET: {
      break;
    }
  }
}

void CMsgProtoBufHeader::CopyFrom(const CMsgProtoBufHeader& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgProtoBufHeader::IsInitialized() const {
  return true;
}

void CMsgProtoBufHeader::InternalSwap(CMsgProtoBufHeader* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  forward_to_sysid_.InternalSwap(&other->forward_to_sysid_);
  exclude_client_sessionids_.InternalSwap(&other->exclude_client_sessionids_);
  target_job_name_.Swap(&other->target_job_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  error_message_.Swap(&other->error_message_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  debug_source_.Swap(&other->debug_source_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  wg_token_.Swap(&other->wg_token_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  webui_auth_key_.Swap(&other->webui_auth_key_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgProtoBufHeader, trace_tag_)
      + sizeof(CMsgProtoBufHeader::trace_tag_)
      - PROTOBUF_FIELD_OFFSET(CMsgProtoBufHeader, routing_gc_)>(
          reinterpret_cast<char*>(&routing_gc_),
          reinterpret_cast<char*>(&other->routing_gc_));
  swap(timeout_ms_, other->timeout_ms_);
  swap(jobid_source_, other->jobid_source_);
  swap(jobid_target_, other->jobid_target_);
  swap(eresult_, other->eresult_);
  swap(transport_error_, other->transport_error_);
  swap(messageid_, other->messageid_);
  swap(ip_addr_, other->ip_addr_);
  swap(_oneof_case_[0], other->_oneof_case_[0]);
}

std::string CMsgProtoBufHeader::GetTypeName() const {
  return "CMsgProtoBufHeader";
}

class CMsgKubeRPCPacket_Hdr::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgKubeRPCPacket_Hdr>()._has_bits_);
  static void set_has_jobid_source(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_jobid_target(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_eresult(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_target_job_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_error_message(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_reply_address(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

CMsgKubeRPCPacket_Hdr::CMsgKubeRPCPacket_Hdr(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgKubeRPCPacket_Hdr::CMsgKubeRPCPacket_Hdr(const CMsgKubeRPCPacket_Hdr& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  target_job_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_target_job_name()) {
    target_job_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_target_job_name(),
      GetArena());
  }
  error_message_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_error_message()) {
    error_message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_error_message(),
      GetArena());
  }
  reply_address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_reply_address()) {
    reply_address_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_reply_address(),
      GetArena());
  }
  ::memcpy(&eresult_, &from.eresult_,
    static_cast<size_t>(reinterpret_cast<char*>(&jobid_target_) -
    reinterpret_cast<char*>(&eresult_)) + sizeof(jobid_target_));

}

void CMsgKubeRPCPacket_Hdr::SharedCtor() {
target_job_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
error_message_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
reply_address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
eresult_ = 2;
jobid_source_ = PROTOBUF_ULONGLONG(18446744073709551615);
jobid_target_ = PROTOBUF_ULONGLONG(18446744073709551615);
}

CMsgKubeRPCPacket_Hdr::~CMsgKubeRPCPacket_Hdr() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgKubeRPCPacket_Hdr::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  target_job_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  error_message_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  reply_address_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgKubeRPCPacket_Hdr::ArenaDtor(void* object) {
  CMsgKubeRPCPacket_Hdr* _this = reinterpret_cast< CMsgKubeRPCPacket_Hdr* >(object);
  (void)_this;
}
void CMsgKubeRPCPacket_Hdr::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgKubeRPCPacket_Hdr::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgKubeRPCPacket_Hdr::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000003fu) {
    if (cached_has_bits & 0x00000001u) {
      target_job_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      error_message_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      reply_address_.ClearNonDefaultToEmpty();
    }
    eresult_ = 2;
    jobid_source_ = PROTOBUF_ULONGLONG(18446744073709551615);
    jobid_target_ = PROTOBUF_ULONGLONG(18446744073709551615);
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgKubeRPCPacket_Hdr::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 81)) {
          _Internal::set_has_jobid_source(&has_bits);
          jobid_source_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 89)) {
          _Internal::set_has_jobid_target(&has_bits);
          jobid_target_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 98)) {
          auto str = _internal_mutable_target_job_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 104)) {
          _Internal::set_has_eresult(&has_bits);
          eresult_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 114)) {
          auto str = _internal_mutable_error_message();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 42:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          auto str = _internal_mutable_reply_address();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgKubeRPCPacket_Hdr::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(10, this->_internal_jobid_source(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(11, this->_internal_jobid_target(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        12, this->_internal_target_job_name(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(13, this->_internal_eresult(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        14, this->_internal_error_message(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        42, this->_internal_reply_address(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgKubeRPCPacket_Hdr::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000003fu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_target_job_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_error_message());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_reply_address());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_eresult());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 + 8;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgKubeRPCPacket_Hdr::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgKubeRPCPacket_Hdr*>(
      &from));
}

void CMsgKubeRPCPacket_Hdr::MergeFrom(const CMsgKubeRPCPacket_Hdr& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000003fu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_target_job_name(from._internal_target_job_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_error_message(from._internal_error_message());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_reply_address(from._internal_reply_address());
    }
    if (cached_has_bits & 0x00000008u) {
      eresult_ = from.eresult_;
    }
    if (cached_has_bits & 0x00000010u) {
      jobid_source_ = from.jobid_source_;
    }
    if (cached_has_bits & 0x00000020u) {
      jobid_target_ = from.jobid_target_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgKubeRPCPacket_Hdr::CopyFrom(const CMsgKubeRPCPacket_Hdr& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgKubeRPCPacket_Hdr::IsInitialized() const {
  return true;
}

void CMsgKubeRPCPacket_Hdr::InternalSwap(CMsgKubeRPCPacket_Hdr* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  target_job_name_.Swap(&other->target_job_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  error_message_.Swap(&other->error_message_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  reply_address_.Swap(&other->reply_address_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(eresult_, other->eresult_);
  swap(jobid_source_, other->jobid_source_);
  swap(jobid_target_, other->jobid_target_);
}

std::string CMsgKubeRPCPacket_Hdr::GetTypeName() const {
  return "CMsgKubeRPCPacket.Hdr";
}

class CMsgKubeRPCPacket::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgKubeRPCPacket>()._has_bits_);
  static const ::CMsgKubeRPCPacket_Hdr& hdr(const CMsgKubeRPCPacket* msg);
  static void set_has_hdr(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_payload(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

const ::CMsgKubeRPCPacket_Hdr&
CMsgKubeRPCPacket::_Internal::hdr(const CMsgKubeRPCPacket* msg) {
  return *msg->hdr_;
}
CMsgKubeRPCPacket::CMsgKubeRPCPacket(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgKubeRPCPacket::CMsgKubeRPCPacket(const CMsgKubeRPCPacket& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  payload_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_payload()) {
    payload_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_payload(),
      GetArena());
  }
  if (from._internal_has_hdr()) {
    hdr_ = new ::CMsgKubeRPCPacket_Hdr(*from.hdr_);
  } else {
    hdr_ = nullptr;
  }

}

void CMsgKubeRPCPacket::SharedCtor() {
payload_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
hdr_ = nullptr;
}

CMsgKubeRPCPacket::~CMsgKubeRPCPacket() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgKubeRPCPacket::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  payload_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete hdr_;
}

void CMsgKubeRPCPacket::ArenaDtor(void* object) {
  CMsgKubeRPCPacket* _this = reinterpret_cast< CMsgKubeRPCPacket* >(object);
  (void)_this;
}
void CMsgKubeRPCPacket::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgKubeRPCPacket::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgKubeRPCPacket::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      payload_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      GOOGLE_DCHECK(hdr_ != nullptr);
      hdr_->Clear();
    }
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgKubeRPCPacket::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr = ctx->ParseMessage(_internal_mutable_hdr(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_payload();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgKubeRPCPacket::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        1, _Internal::hdr(this), target, stream);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_payload(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgKubeRPCPacket::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_payload());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *hdr_);
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgKubeRPCPacket::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgKubeRPCPacket*>(
      &from));
}

void CMsgKubeRPCPacket::MergeFrom(const CMsgKubeRPCPacket& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_payload(from._internal_payload());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_mutable_hdr()->::CMsgKubeRPCPacket_Hdr::MergeFrom(from._internal_hdr());
    }
  }
}

void CMsgKubeRPCPacket::CopyFrom(const CMsgKubeRPCPacket& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgKubeRPCPacket::IsInitialized() const {
  return true;
}

void CMsgKubeRPCPacket::InternalSwap(CMsgKubeRPCPacket* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  payload_.Swap(&other->payload_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(hdr_, other->hdr_);
}

std::string CMsgKubeRPCPacket::GetTypeName() const {
  return "CMsgKubeRPCPacket";
}

class CMsgMulti::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgMulti>()._has_bits_);
  static void set_has_size_unzipped(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_message_body(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgMulti::CMsgMulti(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgMulti::CMsgMulti(const CMsgMulti& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  message_body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_message_body()) {
    message_body_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_message_body(),
      GetArena());
  }
  size_unzipped_ = from.size_unzipped_;

}

void CMsgMulti::SharedCtor() {
message_body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
size_unzipped_ = 0u;
}

CMsgMulti::~CMsgMulti() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgMulti::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  message_body_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgMulti::ArenaDtor(void* object) {
  CMsgMulti* _this = reinterpret_cast< CMsgMulti* >(object);
  (void)_this;
}
void CMsgMulti::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgMulti::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgMulti::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    message_body_.ClearNonDefaultToEmpty();
  }
  size_unzipped_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgMulti::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_size_unzipped(&has_bits);
          size_unzipped_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_message_body();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgMulti::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_size_unzipped(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_message_body(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgMulti::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_message_body());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_size_unzipped());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgMulti::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgMulti*>(
      &from));
}

void CMsgMulti::MergeFrom(const CMsgMulti& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_message_body(from._internal_message_body());
    }
    if (cached_has_bits & 0x00000002u) {
      size_unzipped_ = from.size_unzipped_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgMulti::CopyFrom(const CMsgMulti& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgMulti::IsInitialized() const {
  return true;
}

void CMsgMulti::InternalSwap(CMsgMulti* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  message_body_.Swap(&other->message_body_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(size_unzipped_, other->size_unzipped_);
}

std::string CMsgMulti::GetTypeName() const {
  return "CMsgMulti";
}

class CMsgProtobufWrapped::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgProtobufWrapped>()._has_bits_);
  static void set_has_message_body(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgProtobufWrapped::CMsgProtobufWrapped(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgProtobufWrapped::CMsgProtobufWrapped(const CMsgProtobufWrapped& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  message_body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_message_body()) {
    message_body_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_message_body(),
      GetArena());
  }

}

void CMsgProtobufWrapped::SharedCtor() {
message_body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

CMsgProtobufWrapped::~CMsgProtobufWrapped() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgProtobufWrapped::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  message_body_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgProtobufWrapped::ArenaDtor(void* object) {
  CMsgProtobufWrapped* _this = reinterpret_cast< CMsgProtobufWrapped* >(object);
  (void)_this;
}
void CMsgProtobufWrapped::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgProtobufWrapped::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgProtobufWrapped::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    message_body_.ClearNonDefaultToEmpty();
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgProtobufWrapped::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_message_body();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgProtobufWrapped::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        1, this->_internal_message_body(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgProtobufWrapped::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_message_body());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgProtobufWrapped::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgProtobufWrapped*>(
      &from));
}

void CMsgProtobufWrapped::MergeFrom(const CMsgProtobufWrapped& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_message_body()) {
    _internal_set_message_body(from._internal_message_body());
  }
}

void CMsgProtobufWrapped::CopyFrom(const CMsgProtobufWrapped& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgProtobufWrapped::IsInitialized() const {
  return true;
}

void CMsgProtobufWrapped::InternalSwap(CMsgProtobufWrapped* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  message_body_.Swap(&other->message_body_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

std::string CMsgProtobufWrapped::GetTypeName() const {
  return "CMsgProtobufWrapped";
}

class CMsgAuthTicket::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgAuthTicket>()._has_bits_);
  static void set_has_estate(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_eresult(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_gameid(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_h_steam_pipe(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_ticket_crc(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_ticket(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_server_secret(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_ticket_type(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
};

CMsgAuthTicket::CMsgAuthTicket(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgAuthTicket::CMsgAuthTicket(const CMsgAuthTicket& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_ticket()) {
    ticket_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_ticket(),
      GetArena());
  }
  server_secret_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_server_secret()) {
    server_secret_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_server_secret(),
      GetArena());
  }
  ::memcpy(&steamid_, &from.steamid_,
    static_cast<size_t>(reinterpret_cast<char*>(&eresult_) -
    reinterpret_cast<char*>(&steamid_)) + sizeof(eresult_));

}

void CMsgAuthTicket::SharedCtor() {
ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
server_secret_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&steamid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&ticket_type_) -
    reinterpret_cast<char*>(&steamid_)) + sizeof(ticket_type_));
eresult_ = 2u;
}

CMsgAuthTicket::~CMsgAuthTicket() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgAuthTicket::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  ticket_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  server_secret_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgAuthTicket::ArenaDtor(void* object) {
  CMsgAuthTicket* _this = reinterpret_cast< CMsgAuthTicket* >(object);
  (void)_this;
}
void CMsgAuthTicket::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgAuthTicket::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgAuthTicket::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      ticket_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      server_secret_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x000000fcu) {
    ::memset(&steamid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&ticket_type_) -
        reinterpret_cast<char*>(&steamid_)) + sizeof(ticket_type_));
  }
  eresult_ = 2u;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgAuthTicket::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_estate(&has_bits);
          estate_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_eresult(&has_bits);
          eresult_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 25)) {
          _Internal::set_has_steamid(&has_bits);
          steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 33)) {
          _Internal::set_has_gameid(&has_bits);
          gameid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_h_steam_pipe(&has_bits);
          h_steam_pipe_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_ticket_crc(&has_bits);
          ticket_crc_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_ticket();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 66)) {
          auto str = _internal_mutable_server_secret();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_ticket_type(&has_bits);
          ticket_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgAuthTicket::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_estate(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_eresult(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(3, this->_internal_steamid(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(4, this->_internal_gameid(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(5, this->_internal_h_steam_pipe(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_ticket_crc(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        7, this->_internal_ticket(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteBytesMaybeAliased(
        8, this->_internal_server_secret(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(9, this->_internal_ticket_type(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgAuthTicket::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_ticket());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_server_secret());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_estate());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_h_steam_pipe());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_ticket_crc());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_ticket_type());
    }

  }

  if (cached_has_bits & 0x00000100u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_eresult());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgAuthTicket::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgAuthTicket*>(
      &from));
}

void CMsgAuthTicket::MergeFrom(const CMsgAuthTicket& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_ticket(from._internal_ticket());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_server_secret(from._internal_server_secret());
    }
    if (cached_has_bits & 0x00000004u) {
      steamid_ = from.steamid_;
    }
    if (cached_has_bits & 0x00000008u) {
      estate_ = from.estate_;
    }
    if (cached_has_bits & 0x00000010u) {
      h_steam_pipe_ = from.h_steam_pipe_;
    }
    if (cached_has_bits & 0x00000020u) {
      gameid_ = from.gameid_;
    }
    if (cached_has_bits & 0x00000040u) {
      ticket_crc_ = from.ticket_crc_;
    }
    if (cached_has_bits & 0x00000080u) {
      ticket_type_ = from.ticket_type_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00000100u) {
    _internal_set_eresult(from._internal_eresult());
  }
}

void CMsgAuthTicket::CopyFrom(const CMsgAuthTicket& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgAuthTicket::IsInitialized() const {
  return true;
}

void CMsgAuthTicket::InternalSwap(CMsgAuthTicket* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ticket_.Swap(&other->ticket_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  server_secret_.Swap(&other->server_secret_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgAuthTicket, ticket_type_)
      + sizeof(CMsgAuthTicket::ticket_type_)
      - PROTOBUF_FIELD_OFFSET(CMsgAuthTicket, steamid_)>(
          reinterpret_cast<char*>(&steamid_),
          reinterpret_cast<char*>(&other->steamid_));
  swap(eresult_, other->eresult_);
}

std::string CMsgAuthTicket::GetTypeName() const {
  return "CMsgAuthTicket";
}

class CCDDBAppDetailCommon::_Internal {
 public:
  using HasBits = decltype(std::declval<CCDDBAppDetailCommon>()._has_bits_);
  static void set_has_appid(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_icon(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_tool(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_demo(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_media(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_community_visible_stats(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_friendly_name(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_propagation(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_has_adult_content(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_is_visible_in_steam_china(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_app_type(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_has_adult_content_sex(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_has_adult_content_violence(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
};

CCDDBAppDetailCommon::CCDDBAppDetailCommon(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  content_descriptorids_(arena),
  content_descriptorids_including_dlc_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CCDDBAppDetailCommon::CCDDBAppDetailCommon(const CCDDBAppDetailCommon& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_),
      content_descriptorids_(from.content_descriptorids_),
      content_descriptorids_including_dlc_(from.content_descriptorids_including_dlc_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_name()) {
    name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_name(),
      GetArena());
  }
  icon_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_icon()) {
    icon_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_icon(),
      GetArena());
  }
  friendly_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_friendly_name()) {
    friendly_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_friendly_name(),
      GetArena());
  }
  propagation_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_propagation()) {
    propagation_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_propagation(),
      GetArena());
  }
  ::memcpy(&appid_, &from.appid_,
    static_cast<size_t>(reinterpret_cast<char*>(&has_adult_content_violence_) -
    reinterpret_cast<char*>(&appid_)) + sizeof(has_adult_content_violence_));

}

void CCDDBAppDetailCommon::SharedCtor() {
name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
icon_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
friendly_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
propagation_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&appid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&has_adult_content_violence_) -
    reinterpret_cast<char*>(&appid_)) + sizeof(has_adult_content_violence_));
}

CCDDBAppDetailCommon::~CCDDBAppDetailCommon() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CCDDBAppDetailCommon::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  icon_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  friendly_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  propagation_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CCDDBAppDetailCommon::ArenaDtor(void* object) {
  CCDDBAppDetailCommon* _this = reinterpret_cast< CCDDBAppDetailCommon* >(object);
  (void)_this;
}
void CCDDBAppDetailCommon::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CCDDBAppDetailCommon::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CCDDBAppDetailCommon::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  content_descriptorids_.Clear();
  content_descriptorids_including_dlc_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000000fu) {
    if (cached_has_bits & 0x00000001u) {
      name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      icon_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      friendly_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      propagation_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x000000f0u) {
    ::memset(&appid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&media_) -
        reinterpret_cast<char*>(&appid_)) + sizeof(media_));
  }
  if (cached_has_bits & 0x00003f00u) {
    ::memset(&community_visible_stats_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&has_adult_content_violence_) -
        reinterpret_cast<char*>(&community_visible_stats_)) + sizeof(has_adult_content_violence_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CCDDBAppDetailCommon::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_appid(&has_bits);
          appid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_icon();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_tool(&has_bits);
          tool_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_demo(&has_bits);
          demo_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_media(&has_bits);
          media_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_community_visible_stats(&has_bits);
          community_visible_stats_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          auto str = _internal_mutable_friendly_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 90)) {
          auto str = _internal_mutable_propagation();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 96)) {
          _Internal::set_has_has_adult_content(&has_bits);
          has_adult_content_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 104)) {
          _Internal::set_has_is_visible_in_steam_china(&has_bits);
          is_visible_in_steam_china_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 112)) {
          _Internal::set_has_app_type(&has_bits);
          app_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 120)) {
          _Internal::set_has_has_adult_content_sex(&has_bits);
          has_adult_content_sex_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 128)) {
          _Internal::set_has_has_adult_content_violence(&has_bits);
          has_adult_content_violence_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          ptr -= 2;
          do {
            ptr += 2;
            _internal_add_content_descriptorids(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<136>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 138) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedUInt32Parser(_internal_mutable_content_descriptorids(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 18:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          ptr -= 2;
          do {
            ptr += 2;
            _internal_add_content_descriptorids_including_dlc(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<144>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 146) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedUInt32Parser(_internal_mutable_content_descriptorids_including_dlc(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CCDDBAppDetailCommon::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_appid(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_name(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_icon(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(6, this->_internal_tool(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(7, this->_internal_demo(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(8, this->_internal_media(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(9, this->_internal_community_visible_stats(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        10, this->_internal_friendly_name(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteStringMaybeAliased(
        11, this->_internal_propagation(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(12, this->_internal_has_adult_content(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(13, this->_internal_is_visible_in_steam_china(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(14, this->_internal_app_type(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(15, this->_internal_has_adult_content_sex(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(16, this->_internal_has_adult_content_violence(), target);
  }

  for (int i = 0, n = this->_internal_content_descriptorids_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(17, this->_internal_content_descriptorids(i), target);
  }

  for (int i = 0, n = this->_internal_content_descriptorids_including_dlc_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(18, this->_internal_content_descriptorids_including_dlc(i), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CCDDBAppDetailCommon::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      UInt32Size(this->content_descriptorids_);
    total_size += 2 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_content_descriptorids_size());
    total_size += data_size;
  }

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      UInt32Size(this->content_descriptorids_including_dlc_);
    total_size += 2 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_content_descriptorids_including_dlc_size());
    total_size += data_size;
  }

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_icon());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_friendly_name());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_propagation());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_appid());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 + 1;
    }

  }
  if (cached_has_bits & 0x00003f00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_app_type());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 2 + 1;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CCDDBAppDetailCommon::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CCDDBAppDetailCommon*>(
      &from));
}

void CCDDBAppDetailCommon::MergeFrom(const CCDDBAppDetailCommon& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  content_descriptorids_.MergeFrom(from.content_descriptorids_);
  content_descriptorids_including_dlc_.MergeFrom(from.content_descriptorids_including_dlc_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_name(from._internal_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_icon(from._internal_icon());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_friendly_name(from._internal_friendly_name());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_propagation(from._internal_propagation());
    }
    if (cached_has_bits & 0x00000010u) {
      appid_ = from.appid_;
    }
    if (cached_has_bits & 0x00000020u) {
      tool_ = from.tool_;
    }
    if (cached_has_bits & 0x00000040u) {
      demo_ = from.demo_;
    }
    if (cached_has_bits & 0x00000080u) {
      media_ = from.media_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00003f00u) {
    if (cached_has_bits & 0x00000100u) {
      community_visible_stats_ = from.community_visible_stats_;
    }
    if (cached_has_bits & 0x00000200u) {
      app_type_ = from.app_type_;
    }
    if (cached_has_bits & 0x00000400u) {
      has_adult_content_ = from.has_adult_content_;
    }
    if (cached_has_bits & 0x00000800u) {
      is_visible_in_steam_china_ = from.is_visible_in_steam_china_;
    }
    if (cached_has_bits & 0x00001000u) {
      has_adult_content_sex_ = from.has_adult_content_sex_;
    }
    if (cached_has_bits & 0x00002000u) {
      has_adult_content_violence_ = from.has_adult_content_violence_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CCDDBAppDetailCommon::CopyFrom(const CCDDBAppDetailCommon& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CCDDBAppDetailCommon::IsInitialized() const {
  return true;
}

void CCDDBAppDetailCommon::InternalSwap(CCDDBAppDetailCommon* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  content_descriptorids_.InternalSwap(&other->content_descriptorids_);
  content_descriptorids_including_dlc_.InternalSwap(&other->content_descriptorids_including_dlc_);
  name_.Swap(&other->name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  icon_.Swap(&other->icon_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  friendly_name_.Swap(&other->friendly_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  propagation_.Swap(&other->propagation_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CCDDBAppDetailCommon, has_adult_content_violence_)
      + sizeof(CCDDBAppDetailCommon::has_adult_content_violence_)
      - PROTOBUF_FIELD_OFFSET(CCDDBAppDetailCommon, appid_)>(
          reinterpret_cast<char*>(&appid_),
          reinterpret_cast<char*>(&other->appid_));
}

std::string CCDDBAppDetailCommon::GetTypeName() const {
  return "CCDDBAppDetailCommon";
}

class CMsgAppRights::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgAppRights>()._has_bits_);
  static void set_has_edit_info(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_publish(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_view_error_data(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_download(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_upload_cdkeys(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_generate_cdkeys(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_view_financials(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_manage_ceg(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_manage_signing(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_manage_cdkeys(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_edit_marketing(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_economy_support(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_economy_support_supervisor(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_manage_pricing(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_broadcast_live(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_view_marketing_traffic(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_edit_store_display_content(HasBits* has_bits) {
    (*has_bits)[0] |= 65536u;
  }
};

CMsgAppRights::CMsgAppRights(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgAppRights::CMsgAppRights(const CMsgAppRights& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&edit_info_, &from.edit_info_,
    static_cast<size_t>(reinterpret_cast<char*>(&edit_store_display_content_) -
    reinterpret_cast<char*>(&edit_info_)) + sizeof(edit_store_display_content_));

}

void CMsgAppRights::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&edit_info_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&edit_store_display_content_) -
    reinterpret_cast<char*>(&edit_info_)) + sizeof(edit_store_display_content_));
}

CMsgAppRights::~CMsgAppRights() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgAppRights::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgAppRights::ArenaDtor(void* object) {
  CMsgAppRights* _this = reinterpret_cast< CMsgAppRights* >(object);
  (void)_this;
}
void CMsgAppRights::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgAppRights::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgAppRights::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    ::memset(&edit_info_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&manage_ceg_) -
        reinterpret_cast<char*>(&edit_info_)) + sizeof(manage_ceg_));
  }
  if (cached_has_bits & 0x0000ff00u) {
    ::memset(&manage_signing_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&view_marketing_traffic_) -
        reinterpret_cast<char*>(&manage_signing_)) + sizeof(view_marketing_traffic_));
  }
  edit_store_display_content_ = false;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgAppRights::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_edit_info(&has_bits);
          edit_info_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_publish(&has_bits);
          publish_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_view_error_data(&has_bits);
          view_error_data_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_download(&has_bits);
          download_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_upload_cdkeys(&has_bits);
          upload_cdkeys_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_generate_cdkeys(&has_bits);
          generate_cdkeys_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_view_financials(&has_bits);
          view_financials_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_manage_ceg(&has_bits);
          manage_ceg_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_manage_signing(&has_bits);
          manage_signing_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_manage_cdkeys(&has_bits);
          manage_cdkeys_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 88)) {
          _Internal::set_has_edit_marketing(&has_bits);
          edit_marketing_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 96)) {
          _Internal::set_has_economy_support(&has_bits);
          economy_support_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 104)) {
          _Internal::set_has_economy_support_supervisor(&has_bits);
          economy_support_supervisor_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 112)) {
          _Internal::set_has_manage_pricing(&has_bits);
          manage_pricing_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 120)) {
          _Internal::set_has_broadcast_live(&has_bits);
          broadcast_live_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 128)) {
          _Internal::set_has_view_marketing_traffic(&has_bits);
          view_marketing_traffic_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_edit_store_display_content(&has_bits);
          edit_store_display_content_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgAppRights::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(1, this->_internal_edit_info(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(2, this->_internal_publish(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(3, this->_internal_view_error_data(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(4, this->_internal_download(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(5, this->_internal_upload_cdkeys(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(6, this->_internal_generate_cdkeys(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(7, this->_internal_view_financials(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(8, this->_internal_manage_ceg(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(9, this->_internal_manage_signing(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(10, this->_internal_manage_cdkeys(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(11, this->_internal_edit_marketing(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(12, this->_internal_economy_support(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(13, this->_internal_economy_support_supervisor(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(14, this->_internal_manage_pricing(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(15, this->_internal_broadcast_live(), target);
  }

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(16, this->_internal_view_marketing_traffic(), target);
  }

  if (cached_has_bits & 0x00010000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(17, this->_internal_edit_store_display_content(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgAppRights::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 + 1;
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 + 1;
    }

  }

  if (cached_has_bits & 0x00010000u) {
    total_size += 2 + 1;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgAppRights::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgAppRights*>(
      &from));
}

void CMsgAppRights::MergeFrom(const CMsgAppRights& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      edit_info_ = from.edit_info_;
    }
    if (cached_has_bits & 0x00000002u) {
      publish_ = from.publish_;
    }
    if (cached_has_bits & 0x00000004u) {
      view_error_data_ = from.view_error_data_;
    }
    if (cached_has_bits & 0x00000008u) {
      download_ = from.download_;
    }
    if (cached_has_bits & 0x00000010u) {
      upload_cdkeys_ = from.upload_cdkeys_;
    }
    if (cached_has_bits & 0x00000020u) {
      generate_cdkeys_ = from.generate_cdkeys_;
    }
    if (cached_has_bits & 0x00000040u) {
      view_financials_ = from.view_financials_;
    }
    if (cached_has_bits & 0x00000080u) {
      manage_ceg_ = from.manage_ceg_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      manage_signing_ = from.manage_signing_;
    }
    if (cached_has_bits & 0x00000200u) {
      manage_cdkeys_ = from.manage_cdkeys_;
    }
    if (cached_has_bits & 0x00000400u) {
      edit_marketing_ = from.edit_marketing_;
    }
    if (cached_has_bits & 0x00000800u) {
      economy_support_ = from.economy_support_;
    }
    if (cached_has_bits & 0x00001000u) {
      economy_support_supervisor_ = from.economy_support_supervisor_;
    }
    if (cached_has_bits & 0x00002000u) {
      manage_pricing_ = from.manage_pricing_;
    }
    if (cached_has_bits & 0x00004000u) {
      broadcast_live_ = from.broadcast_live_;
    }
    if (cached_has_bits & 0x00008000u) {
      view_marketing_traffic_ = from.view_marketing_traffic_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00010000u) {
    _internal_set_edit_store_display_content(from._internal_edit_store_display_content());
  }
}

void CMsgAppRights::CopyFrom(const CMsgAppRights& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgAppRights::IsInitialized() const {
  return true;
}

void CMsgAppRights::InternalSwap(CMsgAppRights* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgAppRights, edit_store_display_content_)
      + sizeof(CMsgAppRights::edit_store_display_content_)
      - PROTOBUF_FIELD_OFFSET(CMsgAppRights, edit_info_)>(
          reinterpret_cast<char*>(&edit_info_),
          reinterpret_cast<char*>(&other->edit_info_));
}

std::string CMsgAppRights::GetTypeName() const {
  return "CMsgAppRights";
}

class CCuratorPreferences::_Internal {
 public:
  using HasBits = decltype(std::declval<CCuratorPreferences>()._has_bits_);
  static void set_has_supported_languages(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_platform_windows(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_platform_mac(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_platform_linux(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_vr_content(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_adult_content_violence(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_adult_content_sex(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_timestamp_updated(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_website_title(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_website_url(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_discussion_url(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_show_broadcast(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
};

CCuratorPreferences::CCuratorPreferences(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  tagids_curated_(arena),
  tagids_filtered_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CCuratorPreferences::CCuratorPreferences(const CCuratorPreferences& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_),
      tagids_curated_(from.tagids_curated_),
      tagids_filtered_(from.tagids_filtered_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  website_title_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_website_title()) {
    website_title_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_website_title(),
      GetArena());
  }
  website_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_website_url()) {
    website_url_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_website_url(),
      GetArena());
  }
  discussion_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_discussion_url()) {
    discussion_url_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_discussion_url(),
      GetArena());
  }
  ::memcpy(&supported_languages_, &from.supported_languages_,
    static_cast<size_t>(reinterpret_cast<char*>(&show_broadcast_) -
    reinterpret_cast<char*>(&supported_languages_)) + sizeof(show_broadcast_));

}

void CCuratorPreferences::SharedCtor() {
website_title_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
website_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
discussion_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&supported_languages_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&show_broadcast_) -
    reinterpret_cast<char*>(&supported_languages_)) + sizeof(show_broadcast_));
}

CCuratorPreferences::~CCuratorPreferences() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CCuratorPreferences::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  website_title_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  website_url_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  discussion_url_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CCuratorPreferences::ArenaDtor(void* object) {
  CCuratorPreferences* _this = reinterpret_cast< CCuratorPreferences* >(object);
  (void)_this;
}
void CCuratorPreferences::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CCuratorPreferences::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CCuratorPreferences::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  tagids_curated_.Clear();
  tagids_filtered_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      website_title_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      website_url_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      discussion_url_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x000000f8u) {
    ::memset(&supported_languages_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&vr_content_) -
        reinterpret_cast<char*>(&supported_languages_)) + sizeof(vr_content_));
  }
  if (cached_has_bits & 0x00000f00u) {
    ::memset(&timestamp_updated_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&show_broadcast_) -
        reinterpret_cast<char*>(&timestamp_updated_)) + sizeof(show_broadcast_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CCuratorPreferences::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_supported_languages(&has_bits);
          supported_languages_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_platform_windows(&has_bits);
          platform_windows_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_platform_mac(&has_bits);
          platform_mac_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_platform_linux(&has_bits);
          platform_linux_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_vr_content(&has_bits);
          vr_content_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_adult_content_violence(&has_bits);
          adult_content_violence_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_adult_content_sex(&has_bits);
          adult_content_sex_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_timestamp_updated(&has_bits);
          timestamp_updated_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_tagids_curated(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<72>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedUInt32Parser(_internal_mutable_tagids_curated(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          ptr -= 1;
          do {
            ptr += 1;
            _internal_add_tagids_filtered(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<80>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedUInt32Parser(_internal_mutable_tagids_filtered(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 90)) {
          auto str = _internal_mutable_website_title();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 98)) {
          auto str = _internal_mutable_website_url();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 106)) {
          auto str = _internal_mutable_discussion_url();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 112)) {
          _Internal::set_has_show_broadcast(&has_bits);
          show_broadcast_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CCuratorPreferences::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_supported_languages(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(2, this->_internal_platform_windows(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(3, this->_internal_platform_mac(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(4, this->_internal_platform_linux(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(5, this->_internal_vr_content(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(6, this->_internal_adult_content_violence(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(7, this->_internal_adult_content_sex(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(8, this->_internal_timestamp_updated(), target);
  }

  for (int i = 0, n = this->_internal_tagids_curated_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(9, this->_internal_tagids_curated(i), target);
  }

  for (int i = 0, n = this->_internal_tagids_filtered_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(10, this->_internal_tagids_filtered(i), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        11, this->_internal_website_title(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        12, this->_internal_website_url(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        13, this->_internal_discussion_url(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(14, this->_internal_show_broadcast(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CCuratorPreferences::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      UInt32Size(this->tagids_curated_);
    total_size += 1 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_tagids_curated_size());
    total_size += data_size;
  }

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      UInt32Size(this->tagids_filtered_);
    total_size += 1 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_tagids_filtered_size());
    total_size += data_size;
  }

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_website_title());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_website_url());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_discussion_url());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_supported_languages());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 + 1;
    }

  }
  if (cached_has_bits & 0x00000f00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_timestamp_updated());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 + 1;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CCuratorPreferences::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CCuratorPreferences*>(
      &from));
}

void CCuratorPreferences::MergeFrom(const CCuratorPreferences& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  tagids_curated_.MergeFrom(from.tagids_curated_);
  tagids_filtered_.MergeFrom(from.tagids_filtered_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_website_title(from._internal_website_title());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_website_url(from._internal_website_url());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_discussion_url(from._internal_discussion_url());
    }
    if (cached_has_bits & 0x00000008u) {
      supported_languages_ = from.supported_languages_;
    }
    if (cached_has_bits & 0x00000010u) {
      platform_windows_ = from.platform_windows_;
    }
    if (cached_has_bits & 0x00000020u) {
      platform_mac_ = from.platform_mac_;
    }
    if (cached_has_bits & 0x00000040u) {
      platform_linux_ = from.platform_linux_;
    }
    if (cached_has_bits & 0x00000080u) {
      vr_content_ = from.vr_content_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00000f00u) {
    if (cached_has_bits & 0x00000100u) {
      timestamp_updated_ = from.timestamp_updated_;
    }
    if (cached_has_bits & 0x00000200u) {
      adult_content_violence_ = from.adult_content_violence_;
    }
    if (cached_has_bits & 0x00000400u) {
      adult_content_sex_ = from.adult_content_sex_;
    }
    if (cached_has_bits & 0x00000800u) {
      show_broadcast_ = from.show_broadcast_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CCuratorPreferences::CopyFrom(const CCuratorPreferences& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CCuratorPreferences::IsInitialized() const {
  return true;
}

void CCuratorPreferences::InternalSwap(CCuratorPreferences* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  tagids_curated_.InternalSwap(&other->tagids_curated_);
  tagids_filtered_.InternalSwap(&other->tagids_filtered_);
  website_title_.Swap(&other->website_title_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  website_url_.Swap(&other->website_url_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  discussion_url_.Swap(&other->discussion_url_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CCuratorPreferences, show_broadcast_)
      + sizeof(CCuratorPreferences::show_broadcast_)
      - PROTOBUF_FIELD_OFFSET(CCuratorPreferences, supported_languages_)>(
          reinterpret_cast<char*>(&supported_languages_),
          reinterpret_cast<char*>(&other->supported_languages_));
}

std::string CCuratorPreferences::GetTypeName() const {
  return "CCuratorPreferences";
}

class CLocalizationToken::_Internal {
 public:
  using HasBits = decltype(std::declval<CLocalizationToken>()._has_bits_);
  static void set_has_language(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_localized_string(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CLocalizationToken::CLocalizationToken(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CLocalizationToken::CLocalizationToken(const CLocalizationToken& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  localized_string_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_localized_string()) {
    localized_string_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_localized_string(),
      GetArena());
  }
  language_ = from.language_;

}

void CLocalizationToken::SharedCtor() {
localized_string_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
language_ = 0u;
}

CLocalizationToken::~CLocalizationToken() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CLocalizationToken::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  localized_string_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CLocalizationToken::ArenaDtor(void* object) {
  CLocalizationToken* _this = reinterpret_cast< CLocalizationToken* >(object);
  (void)_this;
}
void CLocalizationToken::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CLocalizationToken::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CLocalizationToken::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    localized_string_.ClearNonDefaultToEmpty();
  }
  language_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CLocalizationToken::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_language(&has_bits);
          language_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_localized_string();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CLocalizationToken::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_language(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_localized_string(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CLocalizationToken::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_localized_string());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_language());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CLocalizationToken::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CLocalizationToken*>(
      &from));
}

void CLocalizationToken::MergeFrom(const CLocalizationToken& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_localized_string(from._internal_localized_string());
    }
    if (cached_has_bits & 0x00000002u) {
      language_ = from.language_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CLocalizationToken::CopyFrom(const CLocalizationToken& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CLocalizationToken::IsInitialized() const {
  return true;
}

void CLocalizationToken::InternalSwap(CLocalizationToken* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  localized_string_.Swap(&other->localized_string_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(language_, other->language_);
}

std::string CLocalizationToken::GetTypeName() const {
  return "CLocalizationToken";
}

class CClanEventUserNewsTuple::_Internal {
 public:
  using HasBits = decltype(std::declval<CClanEventUserNewsTuple>()._has_bits_);
  static void set_has_clanid(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_event_gid(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_announcement_gid(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_rtime_start(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_rtime_end(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_priority_score(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_type(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_clamp_range_slot(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_appid(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_rtime32_last_modified(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
};

CClanEventUserNewsTuple::CClanEventUserNewsTuple(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CClanEventUserNewsTuple::CClanEventUserNewsTuple(const CClanEventUserNewsTuple& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&event_gid_, &from.event_gid_,
    static_cast<size_t>(reinterpret_cast<char*>(&rtime32_last_modified_) -
    reinterpret_cast<char*>(&event_gid_)) + sizeof(rtime32_last_modified_));

}

void CClanEventUserNewsTuple::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&event_gid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&rtime32_last_modified_) -
    reinterpret_cast<char*>(&event_gid_)) + sizeof(rtime32_last_modified_));
}

CClanEventUserNewsTuple::~CClanEventUserNewsTuple() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CClanEventUserNewsTuple::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CClanEventUserNewsTuple::ArenaDtor(void* object) {
  CClanEventUserNewsTuple* _this = reinterpret_cast< CClanEventUserNewsTuple* >(object);
  (void)_this;
}
void CClanEventUserNewsTuple::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CClanEventUserNewsTuple::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CClanEventUserNewsTuple::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    ::memset(&event_gid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&clamp_range_slot_) -
        reinterpret_cast<char*>(&event_gid_)) + sizeof(clamp_range_slot_));
  }
  if (cached_has_bits & 0x00000300u) {
    ::memset(&appid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&rtime32_last_modified_) -
        reinterpret_cast<char*>(&appid_)) + sizeof(rtime32_last_modified_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CClanEventUserNewsTuple::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_clanid(&has_bits);
          clanid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 17)) {
          _Internal::set_has_event_gid(&has_bits);
          event_gid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 25)) {
          _Internal::set_has_announcement_gid(&has_bits);
          announcement_gid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_rtime_start(&has_bits);
          rtime_start_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_rtime_end(&has_bits);
          rtime_end_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_priority_score(&has_bits);
          priority_score_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_type(&has_bits);
          type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_clamp_range_slot(&has_bits);
          clamp_range_slot_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_appid(&has_bits);
          appid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_rtime32_last_modified(&has_bits);
          rtime32_last_modified_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CClanEventUserNewsTuple::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_clanid(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(2, this->_internal_event_gid(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(3, this->_internal_announcement_gid(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(4, this->_internal_rtime_start(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(5, this->_internal_rtime_end(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_priority_score(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(7, this->_internal_type(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(8, this->_internal_clamp_range_slot(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(9, this->_internal_appid(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(10, this->_internal_rtime32_last_modified(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CClanEventUserNewsTuple::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_clanid());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_start());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_end());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_priority_score());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_type());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_clamp_range_slot());
    }

  }
  if (cached_has_bits & 0x00000300u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_appid());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime32_last_modified());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CClanEventUserNewsTuple::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CClanEventUserNewsTuple*>(
      &from));
}

void CClanEventUserNewsTuple::MergeFrom(const CClanEventUserNewsTuple& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      event_gid_ = from.event_gid_;
    }
    if (cached_has_bits & 0x00000002u) {
      clanid_ = from.clanid_;
    }
    if (cached_has_bits & 0x00000004u) {
      rtime_start_ = from.rtime_start_;
    }
    if (cached_has_bits & 0x00000008u) {
      announcement_gid_ = from.announcement_gid_;
    }
    if (cached_has_bits & 0x00000010u) {
      rtime_end_ = from.rtime_end_;
    }
    if (cached_has_bits & 0x00000020u) {
      priority_score_ = from.priority_score_;
    }
    if (cached_has_bits & 0x00000040u) {
      type_ = from.type_;
    }
    if (cached_has_bits & 0x00000080u) {
      clamp_range_slot_ = from.clamp_range_slot_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00000300u) {
    if (cached_has_bits & 0x00000100u) {
      appid_ = from.appid_;
    }
    if (cached_has_bits & 0x00000200u) {
      rtime32_last_modified_ = from.rtime32_last_modified_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CClanEventUserNewsTuple::CopyFrom(const CClanEventUserNewsTuple& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CClanEventUserNewsTuple::IsInitialized() const {
  return true;
}

void CClanEventUserNewsTuple::InternalSwap(CClanEventUserNewsTuple* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CClanEventUserNewsTuple, rtime32_last_modified_)
      + sizeof(CClanEventUserNewsTuple::rtime32_last_modified_)
      - PROTOBUF_FIELD_OFFSET(CClanEventUserNewsTuple, event_gid_)>(
          reinterpret_cast<char*>(&event_gid_),
          reinterpret_cast<char*>(&other->event_gid_));
}

std::string CClanEventUserNewsTuple::GetTypeName() const {
  return "CClanEventUserNewsTuple";
}

class CClanMatchEventByRange::_Internal {
 public:
  using HasBits = decltype(std::declval<CClanMatchEventByRange>()._has_bits_);
  static void set_has_rtime_before(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_rtime_after(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_qualified(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

CClanMatchEventByRange::CClanMatchEventByRange(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  events_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CClanMatchEventByRange::CClanMatchEventByRange(const CClanMatchEventByRange& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_),
      events_(from.events_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&rtime_before_, &from.rtime_before_,
    static_cast<size_t>(reinterpret_cast<char*>(&qualified_) -
    reinterpret_cast<char*>(&rtime_before_)) + sizeof(qualified_));

}

void CClanMatchEventByRange::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&rtime_before_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&qualified_) -
    reinterpret_cast<char*>(&rtime_before_)) + sizeof(qualified_));
}

CClanMatchEventByRange::~CClanMatchEventByRange() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CClanMatchEventByRange::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CClanMatchEventByRange::ArenaDtor(void* object) {
  CClanMatchEventByRange* _this = reinterpret_cast< CClanMatchEventByRange* >(object);
  (void)_this;
}
void CClanMatchEventByRange::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CClanMatchEventByRange::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CClanMatchEventByRange::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  events_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    ::memset(&rtime_before_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&qualified_) -
        reinterpret_cast<char*>(&rtime_before_)) + sizeof(qualified_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CClanMatchEventByRange::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_rtime_before(&has_bits);
          rtime_before_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_rtime_after(&has_bits);
          rtime_after_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_qualified(&has_bits);
          qualified_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_events(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<34>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CClanMatchEventByRange::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_rtime_before(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_rtime_after(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_qualified(), target);
  }

  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_events_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(4, this->_internal_events(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CClanMatchEventByRange::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  total_size += 1UL * this->_internal_events_size();
  for (const auto& msg : this->events_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_before());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_after());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_qualified());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CClanMatchEventByRange::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CClanMatchEventByRange*>(
      &from));
}

void CClanMatchEventByRange::MergeFrom(const CClanMatchEventByRange& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  events_.MergeFrom(from.events_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      rtime_before_ = from.rtime_before_;
    }
    if (cached_has_bits & 0x00000002u) {
      rtime_after_ = from.rtime_after_;
    }
    if (cached_has_bits & 0x00000004u) {
      qualified_ = from.qualified_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CClanMatchEventByRange::CopyFrom(const CClanMatchEventByRange& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CClanMatchEventByRange::IsInitialized() const {
  return true;
}

void CClanMatchEventByRange::InternalSwap(CClanMatchEventByRange* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  events_.InternalSwap(&other->events_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CClanMatchEventByRange, qualified_)
      + sizeof(CClanMatchEventByRange::qualified_)
      - PROTOBUF_FIELD_OFFSET(CClanMatchEventByRange, rtime_before_)>(
          reinterpret_cast<char*>(&rtime_before_),
          reinterpret_cast<char*>(&other->rtime_before_));
}

std::string CClanMatchEventByRange::GetTypeName() const {
  return "CClanMatchEventByRange";
}

class CCommunity_ClanAnnouncementInfo::_Internal {
 public:
  using HasBits = decltype(std::declval<CCommunity_ClanAnnouncementInfo>()._has_bits_);
  static void set_has_gid(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_clanid(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_posterid(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_headline(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_posttime(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_updatetime(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_body(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_commentcount(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_language(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_hidden(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_forum_topic_id(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_event_gid(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_voteupcount(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_votedowncount(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_ban_check_result(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_banned(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
};

CCommunity_ClanAnnouncementInfo::CCommunity_ClanAnnouncementInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  tags_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CCommunity_ClanAnnouncementInfo::CCommunity_ClanAnnouncementInfo(const CCommunity_ClanAnnouncementInfo& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_),
      tags_(from.tags_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  headline_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_headline()) {
    headline_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_headline(),
      GetArena());
  }
  body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_body()) {
    body_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_body(),
      GetArena());
  }
  ::memcpy(&gid_, &from.gid_,
    static_cast<size_t>(reinterpret_cast<char*>(&ban_check_result_) -
    reinterpret_cast<char*>(&gid_)) + sizeof(ban_check_result_));

}

void CCommunity_ClanAnnouncementInfo::SharedCtor() {
headline_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
body_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&gid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&ban_check_result_) -
    reinterpret_cast<char*>(&gid_)) + sizeof(ban_check_result_));
}

CCommunity_ClanAnnouncementInfo::~CCommunity_ClanAnnouncementInfo() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CCommunity_ClanAnnouncementInfo::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  headline_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  body_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CCommunity_ClanAnnouncementInfo::ArenaDtor(void* object) {
  CCommunity_ClanAnnouncementInfo* _this = reinterpret_cast< CCommunity_ClanAnnouncementInfo* >(object);
  (void)_this;
}
void CCommunity_ClanAnnouncementInfo::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CCommunity_ClanAnnouncementInfo::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CCommunity_ClanAnnouncementInfo::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  tags_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      headline_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      body_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x000000fcu) {
    ::memset(&gid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&commentcount_) -
        reinterpret_cast<char*>(&gid_)) + sizeof(commentcount_));
  }
  if (cached_has_bits & 0x0000ff00u) {
    ::memset(&language_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&ban_check_result_) -
        reinterpret_cast<char*>(&language_)) + sizeof(ban_check_result_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CCommunity_ClanAnnouncementInfo::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_gid(&has_bits);
          gid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_clanid(&has_bits);
          clanid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_posterid(&has_bits);
          posterid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          auto str = _internal_mutable_headline();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_posttime(&has_bits);
          posttime_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_updatetime(&has_bits);
          updatetime_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_body();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_commentcount(&has_bits);
          commentcount_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          ptr -= 1;
          do {
            ptr += 1;
            auto str = _internal_add_tags();
            ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<74>(ptr));
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_language(&has_bits);
          language_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 88)) {
          _Internal::set_has_hidden(&has_bits);
          hidden_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 97)) {
          _Internal::set_has_forum_topic_id(&has_bits);
          forum_topic_id_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 105)) {
          _Internal::set_has_event_gid(&has_bits);
          event_gid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 112)) {
          _Internal::set_has_voteupcount(&has_bits);
          voteupcount_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 120)) {
          _Internal::set_has_votedowncount(&has_bits);
          votedowncount_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 128)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::EBanContentCheckResult_IsValid(val))) {
            _internal_set_ban_check_result(static_cast<::EBanContentCheckResult>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(16, val, mutable_unknown_fields());
          }
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_banned(&has_bits);
          banned_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CCommunity_ClanAnnouncementInfo::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(1, this->_internal_gid(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(2, this->_internal_clanid(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(3, this->_internal_posterid(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_headline(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(5, this->_internal_posttime(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_updatetime(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        7, this->_internal_body(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(8, this->_internal_commentcount(), target);
  }

  for (int i = 0, n = this->_internal_tags_size(); i < n; i++) {
    const auto& s = this->_internal_tags(i);
    target = stream->WriteString(9, s, target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(10, this->_internal_language(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(11, this->_internal_hidden(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(12, this->_internal_forum_topic_id(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(13, this->_internal_event_gid(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(14, this->_internal_voteupcount(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(15, this->_internal_votedowncount(), target);
  }

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      16, this->_internal_ban_check_result(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(17, this->_internal_banned(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CCommunity_ClanAnnouncementInfo::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  total_size += 1 *
      ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(tags_.size());
  for (int i = 0, n = tags_.size(); i < n; i++) {
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
      tags_.Get(i));
  }

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_headline());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_body());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_gid());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_clanid());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_posterid());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_posttime());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_updatetime());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_commentcount());
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_language());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_voteupcount());
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_votedowncount());
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_ban_check_result());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CCommunity_ClanAnnouncementInfo::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CCommunity_ClanAnnouncementInfo*>(
      &from));
}

void CCommunity_ClanAnnouncementInfo::MergeFrom(const CCommunity_ClanAnnouncementInfo& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  tags_.MergeFrom(from.tags_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_headline(from._internal_headline());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_body(from._internal_body());
    }
    if (cached_has_bits & 0x00000004u) {
      gid_ = from.gid_;
    }
    if (cached_has_bits & 0x00000008u) {
      clanid_ = from.clanid_;
    }
    if (cached_has_bits & 0x00000010u) {
      posterid_ = from.posterid_;
    }
    if (cached_has_bits & 0x00000020u) {
      posttime_ = from.posttime_;
    }
    if (cached_has_bits & 0x00000040u) {
      updatetime_ = from.updatetime_;
    }
    if (cached_has_bits & 0x00000080u) {
      commentcount_ = from.commentcount_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      language_ = from.language_;
    }
    if (cached_has_bits & 0x00000200u) {
      forum_topic_id_ = from.forum_topic_id_;
    }
    if (cached_has_bits & 0x00000400u) {
      event_gid_ = from.event_gid_;
    }
    if (cached_has_bits & 0x00000800u) {
      voteupcount_ = from.voteupcount_;
    }
    if (cached_has_bits & 0x00001000u) {
      hidden_ = from.hidden_;
    }
    if (cached_has_bits & 0x00002000u) {
      banned_ = from.banned_;
    }
    if (cached_has_bits & 0x00004000u) {
      votedowncount_ = from.votedowncount_;
    }
    if (cached_has_bits & 0x00008000u) {
      ban_check_result_ = from.ban_check_result_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CCommunity_ClanAnnouncementInfo::CopyFrom(const CCommunity_ClanAnnouncementInfo& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CCommunity_ClanAnnouncementInfo::IsInitialized() const {
  return true;
}

void CCommunity_ClanAnnouncementInfo::InternalSwap(CCommunity_ClanAnnouncementInfo* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  tags_.InternalSwap(&other->tags_);
  headline_.Swap(&other->headline_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  body_.Swap(&other->body_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CCommunity_ClanAnnouncementInfo, ban_check_result_)
      + sizeof(CCommunity_ClanAnnouncementInfo::ban_check_result_)
      - PROTOBUF_FIELD_OFFSET(CCommunity_ClanAnnouncementInfo, gid_)>(
          reinterpret_cast<char*>(&gid_),
          reinterpret_cast<char*>(&other->gid_));
}

std::string CCommunity_ClanAnnouncementInfo::GetTypeName() const {
  return "CCommunity_ClanAnnouncementInfo";
}

class CClanEventData::_Internal {
 public:
  using HasBits = decltype(std::declval<CClanEventData>()._has_bits_);
  static void set_has_gid(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_clan_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_event_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_event_type(HasBits* has_bits) {
    (*has_bits)[0] |= 1073741824u;
  }
  static void set_has_appid(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_server_address(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_server_password(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_rtime32_start_time(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_rtime32_end_time(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_comment_count(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_creator_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_last_update_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_event_notes(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_jsondata(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static const ::CCommunity_ClanAnnouncementInfo& announcement_body(const CClanEventData* msg);
  static void set_has_announcement_body(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_published(HasBits* has_bits) {
    (*has_bits)[0] |= 524288u;
  }
  static void set_has_hidden(HasBits* has_bits) {
    (*has_bits)[0] |= 1048576u;
  }
  static void set_has_rtime32_visibility_start(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_rtime32_visibility_end(HasBits* has_bits) {
    (*has_bits)[0] |= 65536u;
  }
  static void set_has_broadcaster_accountid(HasBits* has_bits) {
    (*has_bits)[0] |= 131072u;
  }
  static void set_has_follower_count(HasBits* has_bits) {
    (*has_bits)[0] |= 262144u;
  }
  static void set_has_ignore_count(HasBits* has_bits) {
    (*has_bits)[0] |= 4194304u;
  }
  static void set_has_forum_topic_id(HasBits* has_bits) {
    (*has_bits)[0] |= 8388608u;
  }
  static void set_has_rtime32_last_modified(HasBits* has_bits) {
    (*has_bits)[0] |= 33554432u;
  }
  static void set_has_news_post_gid(HasBits* has_bits) {
    (*has_bits)[0] |= 16777216u;
  }
  static void set_has_rtime_mod_reviewed(HasBits* has_bits) {
    (*has_bits)[0] |= 67108864u;
  }
  static void set_has_featured_app_tagid(HasBits* has_bits) {
    (*has_bits)[0] |= 134217728u;
  }
  static void set_has_build_id(HasBits* has_bits) {
    (*has_bits)[0] |= 268435456u;
  }
  static void set_has_build_branch(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_unlisted(HasBits* has_bits) {
    (*has_bits)[0] |= 2097152u;
  }
  static void set_has_rtime_created(HasBits* has_bits) {
    (*has_bits)[0] |= 536870912u;
  }
};

const ::CCommunity_ClanAnnouncementInfo&
CClanEventData::_Internal::announcement_body(const CClanEventData* msg) {
  return *msg->announcement_body_;
}
CClanEventData::CClanEventData(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  referenced_appids_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CClanEventData::CClanEventData(const CClanEventData& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_),
      referenced_appids_(from.referenced_appids_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  event_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_event_name()) {
    event_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_event_name(),
      GetArena());
  }
  server_address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_server_address()) {
    server_address_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_server_address(),
      GetArena());
  }
  server_password_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_server_password()) {
    server_password_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_server_password(),
      GetArena());
  }
  event_notes_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_event_notes()) {
    event_notes_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_event_notes(),
      GetArena());
  }
  jsondata_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_jsondata()) {
    jsondata_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_jsondata(),
      GetArena());
  }
  build_branch_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_build_branch()) {
    build_branch_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_build_branch(),
      GetArena());
  }
  if (from._internal_has_announcement_body()) {
    announcement_body_ = new ::CCommunity_ClanAnnouncementInfo(*from.announcement_body_);
  } else {
    announcement_body_ = nullptr;
  }
  ::memcpy(&gid_, &from.gid_,
    static_cast<size_t>(reinterpret_cast<char*>(&event_type_) -
    reinterpret_cast<char*>(&gid_)) + sizeof(event_type_));

}

void CClanEventData::SharedCtor() {
event_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
server_address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
server_password_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
event_notes_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
jsondata_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
build_branch_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&announcement_body_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&rtime_created_) -
    reinterpret_cast<char*>(&announcement_body_)) + sizeof(rtime_created_));
event_type_ = 1;
}

CClanEventData::~CClanEventData() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CClanEventData::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  event_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  server_address_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  server_password_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  event_notes_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  jsondata_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  build_branch_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete announcement_body_;
}

void CClanEventData::ArenaDtor(void* object) {
  CClanEventData* _this = reinterpret_cast< CClanEventData* >(object);
  (void)_this;
}
void CClanEventData::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CClanEventData::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CClanEventData::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  referenced_appids_.Clear();
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000007fu) {
    if (cached_has_bits & 0x00000001u) {
      event_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      server_address_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      server_password_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      event_notes_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000010u) {
      jsondata_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000020u) {
      build_branch_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000040u) {
      GOOGLE_DCHECK(announcement_body_ != nullptr);
      announcement_body_->Clear();
    }
  }
  gid_ = PROTOBUF_ULONGLONG(0);
  if (cached_has_bits & 0x0000ff00u) {
    ::memset(&clan_steamid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&rtime32_visibility_start_) -
        reinterpret_cast<char*>(&clan_steamid_)) + sizeof(rtime32_visibility_start_));
  }
  if (cached_has_bits & 0x00ff0000u) {
    ::memset(&rtime32_visibility_end_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&forum_topic_id_) -
        reinterpret_cast<char*>(&rtime32_visibility_end_)) + sizeof(forum_topic_id_));
  }
  if (cached_has_bits & 0x7f000000u) {
    ::memset(&news_post_gid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&rtime_created_) -
        reinterpret_cast<char*>(&news_post_gid_)) + sizeof(rtime_created_));
    event_type_ = 1;
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CClanEventData::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 9)) {
          _Internal::set_has_gid(&has_bits);
          gid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 17)) {
          _Internal::set_has_clan_steamid(&has_bits);
          clan_steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_event_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::EProtoClanEventType_IsValid(val))) {
            _internal_set_event_type(static_cast<::EProtoClanEventType>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(4, val, mutable_unknown_fields());
          }
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_appid(&has_bits);
          appid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 50)) {
          auto str = _internal_mutable_server_address();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_server_password();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_rtime32_start_time(&has_bits);
          rtime32_start_time_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_rtime32_end_time(&has_bits);
          rtime32_end_time_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_comment_count(&has_bits);
          comment_count_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 89)) {
          _Internal::set_has_creator_steamid(&has_bits);
          creator_steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 97)) {
          _Internal::set_has_last_update_steamid(&has_bits);
          last_update_steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 106)) {
          auto str = _internal_mutable_event_notes();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 114)) {
          auto str = _internal_mutable_jsondata();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 122)) {
          ptr = ctx->ParseMessage(_internal_mutable_announcement_body(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 128)) {
          _Internal::set_has_published(&has_bits);
          published_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_hidden(&has_bits);
          hidden_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 18:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          _Internal::set_has_rtime32_visibility_start(&has_bits);
          rtime32_visibility_start_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 19:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 152)) {
          _Internal::set_has_rtime32_visibility_end(&has_bits);
          rtime32_visibility_end_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 160)) {
          _Internal::set_has_broadcaster_accountid(&has_bits);
          broadcaster_accountid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 21:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 168)) {
          _Internal::set_has_follower_count(&has_bits);
          follower_count_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 22:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 176)) {
          _Internal::set_has_ignore_count(&has_bits);
          ignore_count_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 23:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 185)) {
          _Internal::set_has_forum_topic_id(&has_bits);
          forum_topic_id_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 24:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 192)) {
          _Internal::set_has_rtime32_last_modified(&has_bits);
          rtime32_last_modified_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 25:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 201)) {
          _Internal::set_has_news_post_gid(&has_bits);
          news_post_gid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 26:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 208)) {
          _Internal::set_has_rtime_mod_reviewed(&has_bits);
          rtime_mod_reviewed_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 27:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 216)) {
          _Internal::set_has_featured_app_tagid(&has_bits);
          featured_app_tagid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 28:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 224)) {
          ptr -= 2;
          do {
            ptr += 2;
            _internal_add_referenced_appids(::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr));
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<224>(ptr));
        } else if (static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 226) {
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::PackedUInt32Parser(_internal_mutable_referenced_appids(), ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 29:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 232)) {
          _Internal::set_has_build_id(&has_bits);
          build_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 30:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 242)) {
          auto str = _internal_mutable_build_branch();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 31:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 248)) {
          _Internal::set_has_unlisted(&has_bits);
          unlisted_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 32:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 0)) {
          _Internal::set_has_rtime_created(&has_bits);
          rtime_created_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CClanEventData::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(1, this->_internal_gid(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(2, this->_internal_clan_steamid(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_event_name(), target);
  }

  if (cached_has_bits & 0x40000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      4, this->_internal_event_type(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(5, this->_internal_appid(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        6, this->_internal_server_address(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        7, this->_internal_server_password(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(8, this->_internal_rtime32_start_time(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(9, this->_internal_rtime32_end_time(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(10, this->_internal_comment_count(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(11, this->_internal_creator_steamid(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(12, this->_internal_last_update_steamid(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteStringMaybeAliased(
        13, this->_internal_event_notes(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->WriteStringMaybeAliased(
        14, this->_internal_jsondata(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        15, _Internal::announcement_body(this), target, stream);
  }

  if (cached_has_bits & 0x00080000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(16, this->_internal_published(), target);
  }

  if (cached_has_bits & 0x00100000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(17, this->_internal_hidden(), target);
  }

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(18, this->_internal_rtime32_visibility_start(), target);
  }

  if (cached_has_bits & 0x00010000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(19, this->_internal_rtime32_visibility_end(), target);
  }

  if (cached_has_bits & 0x00020000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(20, this->_internal_broadcaster_accountid(), target);
  }

  if (cached_has_bits & 0x00040000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(21, this->_internal_follower_count(), target);
  }

  if (cached_has_bits & 0x00400000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(22, this->_internal_ignore_count(), target);
  }

  if (cached_has_bits & 0x00800000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(23, this->_internal_forum_topic_id(), target);
  }

  if (cached_has_bits & 0x02000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(24, this->_internal_rtime32_last_modified(), target);
  }

  if (cached_has_bits & 0x01000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(25, this->_internal_news_post_gid(), target);
  }

  if (cached_has_bits & 0x04000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(26, this->_internal_rtime_mod_reviewed(), target);
  }

  if (cached_has_bits & 0x08000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(27, this->_internal_featured_app_tagid(), target);
  }

  for (int i = 0, n = this->_internal_referenced_appids_size(); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(28, this->_internal_referenced_appids(i), target);
  }

  if (cached_has_bits & 0x10000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(29, this->_internal_build_id(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->WriteStringMaybeAliased(
        30, this->_internal_build_branch(), target);
  }

  if (cached_has_bits & 0x00200000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(31, this->_internal_unlisted(), target);
  }

  if (cached_has_bits & 0x20000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(32, this->_internal_rtime_created(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CClanEventData::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  {
    size_t data_size = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      UInt32Size(this->referenced_appids_);
    total_size += 2 *
                  ::PROTOBUF_NAMESPACE_ID::internal::FromIntSize(this->_internal_referenced_appids_size());
    total_size += data_size;
  }

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_event_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_server_address());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_server_password());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_event_notes());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_jsondata());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_build_branch());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *announcement_body_);
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 + 8;
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_appid());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime32_start_time());
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime32_end_time());
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_comment_count());
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime32_visibility_start());
    }

  }
  if (cached_has_bits & 0x00ff0000u) {

    if (cached_has_bits & 0x00010000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime32_visibility_end());
    }

    if (cached_has_bits & 0x00020000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_broadcaster_accountid());
    }

    if (cached_has_bits & 0x00040000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_follower_count());
    }

    if (cached_has_bits & 0x00080000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00100000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00200000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00400000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_ignore_count());
    }

    if (cached_has_bits & 0x00800000u) {
      total_size += 2 + 8;
    }

  }
  if (cached_has_bits & 0x7f000000u) {

    if (cached_has_bits & 0x01000000u) {
      total_size += 2 + 8;
    }

    if (cached_has_bits & 0x02000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime32_last_modified());
    }

    if (cached_has_bits & 0x04000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_mod_reviewed());
    }

    if (cached_has_bits & 0x08000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_featured_app_tagid());
    }

    if (cached_has_bits & 0x10000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_build_id());
    }

    if (cached_has_bits & 0x20000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_created());
    }

    if (cached_has_bits & 0x40000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_event_type());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CClanEventData::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CClanEventData*>(
      &from));
}

void CClanEventData::MergeFrom(const CClanEventData& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  referenced_appids_.MergeFrom(from.referenced_appids_);
  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_event_name(from._internal_event_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_server_address(from._internal_server_address());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_server_password(from._internal_server_password());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_event_notes(from._internal_event_notes());
    }
    if (cached_has_bits & 0x00000010u) {
      _internal_set_jsondata(from._internal_jsondata());
    }
    if (cached_has_bits & 0x00000020u) {
      _internal_set_build_branch(from._internal_build_branch());
    }
    if (cached_has_bits & 0x00000040u) {
      _internal_mutable_announcement_body()->::CCommunity_ClanAnnouncementInfo::MergeFrom(from._internal_announcement_body());
    }
    if (cached_has_bits & 0x00000080u) {
      gid_ = from.gid_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      clan_steamid_ = from.clan_steamid_;
    }
    if (cached_has_bits & 0x00000200u) {
      appid_ = from.appid_;
    }
    if (cached_has_bits & 0x00000400u) {
      rtime32_start_time_ = from.rtime32_start_time_;
    }
    if (cached_has_bits & 0x00000800u) {
      rtime32_end_time_ = from.rtime32_end_time_;
    }
    if (cached_has_bits & 0x00001000u) {
      comment_count_ = from.comment_count_;
    }
    if (cached_has_bits & 0x00002000u) {
      creator_steamid_ = from.creator_steamid_;
    }
    if (cached_has_bits & 0x00004000u) {
      last_update_steamid_ = from.last_update_steamid_;
    }
    if (cached_has_bits & 0x00008000u) {
      rtime32_visibility_start_ = from.rtime32_visibility_start_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00ff0000u) {
    if (cached_has_bits & 0x00010000u) {
      rtime32_visibility_end_ = from.rtime32_visibility_end_;
    }
    if (cached_has_bits & 0x00020000u) {
      broadcaster_accountid_ = from.broadcaster_accountid_;
    }
    if (cached_has_bits & 0x00040000u) {
      follower_count_ = from.follower_count_;
    }
    if (cached_has_bits & 0x00080000u) {
      published_ = from.published_;
    }
    if (cached_has_bits & 0x00100000u) {
      hidden_ = from.hidden_;
    }
    if (cached_has_bits & 0x00200000u) {
      unlisted_ = from.unlisted_;
    }
    if (cached_has_bits & 0x00400000u) {
      ignore_count_ = from.ignore_count_;
    }
    if (cached_has_bits & 0x00800000u) {
      forum_topic_id_ = from.forum_topic_id_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x7f000000u) {
    if (cached_has_bits & 0x01000000u) {
      news_post_gid_ = from.news_post_gid_;
    }
    if (cached_has_bits & 0x02000000u) {
      rtime32_last_modified_ = from.rtime32_last_modified_;
    }
    if (cached_has_bits & 0x04000000u) {
      rtime_mod_reviewed_ = from.rtime_mod_reviewed_;
    }
    if (cached_has_bits & 0x08000000u) {
      featured_app_tagid_ = from.featured_app_tagid_;
    }
    if (cached_has_bits & 0x10000000u) {
      build_id_ = from.build_id_;
    }
    if (cached_has_bits & 0x20000000u) {
      rtime_created_ = from.rtime_created_;
    }
    if (cached_has_bits & 0x40000000u) {
      event_type_ = from.event_type_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CClanEventData::CopyFrom(const CClanEventData& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CClanEventData::IsInitialized() const {
  return true;
}

void CClanEventData::InternalSwap(CClanEventData* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  referenced_appids_.InternalSwap(&other->referenced_appids_);
  event_name_.Swap(&other->event_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  server_address_.Swap(&other->server_address_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  server_password_.Swap(&other->server_password_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  event_notes_.Swap(&other->event_notes_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  jsondata_.Swap(&other->jsondata_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  build_branch_.Swap(&other->build_branch_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CClanEventData, rtime_created_)
      + sizeof(CClanEventData::rtime_created_)
      - PROTOBUF_FIELD_OFFSET(CClanEventData, announcement_body_)>(
          reinterpret_cast<char*>(&announcement_body_),
          reinterpret_cast<char*>(&other->announcement_body_));
  swap(event_type_, other->event_type_);
}

std::string CClanEventData::GetTypeName() const {
  return "CClanEventData";
}

class CBilling_Address::_Internal {
 public:
  using HasBits = decltype(std::declval<CBilling_Address>()._has_bits_);
  static void set_has_first_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_last_name(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_address1(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_address2(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_city(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_us_state(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_country_code(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_postcode(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_zip_plus4(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_phone(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
};

CBilling_Address::CBilling_Address(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CBilling_Address::CBilling_Address(const CBilling_Address& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  first_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_first_name()) {
    first_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_first_name(),
      GetArena());
  }
  last_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_last_name()) {
    last_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_last_name(),
      GetArena());
  }
  address1_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_address1()) {
    address1_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_address1(),
      GetArena());
  }
  address2_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_address2()) {
    address2_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_address2(),
      GetArena());
  }
  city_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_city()) {
    city_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_city(),
      GetArena());
  }
  us_state_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_us_state()) {
    us_state_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_us_state(),
      GetArena());
  }
  country_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_country_code()) {
    country_code_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_country_code(),
      GetArena());
  }
  postcode_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_postcode()) {
    postcode_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_postcode(),
      GetArena());
  }
  phone_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_phone()) {
    phone_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_phone(),
      GetArena());
  }
  zip_plus4_ = from.zip_plus4_;

}

void CBilling_Address::SharedCtor() {
first_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
last_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
address1_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
address2_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
city_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
us_state_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
country_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
postcode_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
phone_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
zip_plus4_ = 0;
}

CBilling_Address::~CBilling_Address() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CBilling_Address::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  first_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  last_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  address1_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  address2_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  city_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  us_state_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  country_code_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  postcode_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  phone_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CBilling_Address::ArenaDtor(void* object) {
  CBilling_Address* _this = reinterpret_cast< CBilling_Address* >(object);
  (void)_this;
}
void CBilling_Address::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CBilling_Address::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CBilling_Address::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      first_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      last_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      address1_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      address2_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000010u) {
      city_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000020u) {
      us_state_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000040u) {
      country_code_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000080u) {
      postcode_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x00000100u) {
    phone_.ClearNonDefaultToEmpty();
  }
  zip_plus4_ = 0;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CBilling_Address::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_first_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_last_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_address1();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          auto str = _internal_mutable_address2();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          auto str = _internal_mutable_city();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 50)) {
          auto str = _internal_mutable_us_state();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_country_code();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 66)) {
          auto str = _internal_mutable_postcode();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_zip_plus4(&has_bits);
          zip_plus4_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          auto str = _internal_mutable_phone();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CBilling_Address::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_first_name(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_last_name(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_address1(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteStringMaybeAliased(
        4, this->_internal_address2(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->WriteStringMaybeAliased(
        5, this->_internal_city(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->WriteStringMaybeAliased(
        6, this->_internal_us_state(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->WriteStringMaybeAliased(
        7, this->_internal_country_code(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->WriteStringMaybeAliased(
        8, this->_internal_postcode(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(9, this->_internal_zip_plus4(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->WriteStringMaybeAliased(
        10, this->_internal_phone(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CBilling_Address::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_first_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_last_name());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_address1());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_address2());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_city());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_us_state());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_country_code());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_postcode());
    }

  }
  if (cached_has_bits & 0x00000300u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_phone());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_zip_plus4());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CBilling_Address::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CBilling_Address*>(
      &from));
}

void CBilling_Address::MergeFrom(const CBilling_Address& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_first_name(from._internal_first_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_last_name(from._internal_last_name());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_address1(from._internal_address1());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_address2(from._internal_address2());
    }
    if (cached_has_bits & 0x00000010u) {
      _internal_set_city(from._internal_city());
    }
    if (cached_has_bits & 0x00000020u) {
      _internal_set_us_state(from._internal_us_state());
    }
    if (cached_has_bits & 0x00000040u) {
      _internal_set_country_code(from._internal_country_code());
    }
    if (cached_has_bits & 0x00000080u) {
      _internal_set_postcode(from._internal_postcode());
    }
  }
  if (cached_has_bits & 0x00000300u) {
    if (cached_has_bits & 0x00000100u) {
      _internal_set_phone(from._internal_phone());
    }
    if (cached_has_bits & 0x00000200u) {
      zip_plus4_ = from.zip_plus4_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CBilling_Address::CopyFrom(const CBilling_Address& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CBilling_Address::IsInitialized() const {
  return true;
}

void CBilling_Address::InternalSwap(CBilling_Address* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  first_name_.Swap(&other->first_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  last_name_.Swap(&other->last_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  address1_.Swap(&other->address1_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  address2_.Swap(&other->address2_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  city_.Swap(&other->city_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  us_state_.Swap(&other->us_state_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  country_code_.Swap(&other->country_code_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  postcode_.Swap(&other->postcode_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  phone_.Swap(&other->phone_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(zip_plus4_, other->zip_plus4_);
}

std::string CBilling_Address::GetTypeName() const {
  return "CBilling_Address";
}

class CPackageReservationStatus::_Internal {
 public:
  using HasBits = decltype(std::declval<CPackageReservationStatus>()._has_bits_);
  static void set_has_packageid(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_reservation_state(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_queue_position(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_total_queue_size(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_reservation_country_code(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_expired(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_time_expires(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_time_reserved(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_rtime_estimated_notification(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_notificaton_token(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_queue_head_position_at_reservation(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_queue_head_position_now(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
};

CPackageReservationStatus::CPackageReservationStatus(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CPackageReservationStatus::CPackageReservationStatus(const CPackageReservationStatus& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  reservation_country_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_reservation_country_code()) {
    reservation_country_code_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_reservation_country_code(),
      GetArena());
  }
  notificaton_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_notificaton_token()) {
    notificaton_token_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_notificaton_token(),
      GetArena());
  }
  ::memcpy(&packageid_, &from.packageid_,
    static_cast<size_t>(reinterpret_cast<char*>(&queue_head_position_now_) -
    reinterpret_cast<char*>(&packageid_)) + sizeof(queue_head_position_now_));

}

void CPackageReservationStatus::SharedCtor() {
reservation_country_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
notificaton_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&packageid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&queue_head_position_now_) -
    reinterpret_cast<char*>(&packageid_)) + sizeof(queue_head_position_now_));
}

CPackageReservationStatus::~CPackageReservationStatus() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CPackageReservationStatus::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  reservation_country_code_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  notificaton_token_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CPackageReservationStatus::ArenaDtor(void* object) {
  CPackageReservationStatus* _this = reinterpret_cast< CPackageReservationStatus* >(object);
  (void)_this;
}
void CPackageReservationStatus::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CPackageReservationStatus::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CPackageReservationStatus::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      reservation_country_code_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      notificaton_token_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x000000fcu) {
    ::memset(&packageid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&time_expires_) -
        reinterpret_cast<char*>(&packageid_)) + sizeof(time_expires_));
  }
  if (cached_has_bits & 0x00000f00u) {
    ::memset(&time_reserved_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&queue_head_position_now_) -
        reinterpret_cast<char*>(&time_reserved_)) + sizeof(queue_head_position_now_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CPackageReservationStatus::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_packageid(&has_bits);
          packageid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_reservation_state(&has_bits);
          reservation_state_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_queue_position(&has_bits);
          queue_position_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_total_queue_size(&has_bits);
          total_queue_size_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          auto str = _internal_mutable_reservation_country_code();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_expired(&has_bits);
          expired_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_time_expires(&has_bits);
          time_expires_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_time_reserved(&has_bits);
          time_reserved_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 72)) {
          _Internal::set_has_rtime_estimated_notification(&has_bits);
          rtime_estimated_notification_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          auto str = _internal_mutable_notificaton_token();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 88)) {
          _Internal::set_has_queue_head_position_at_reservation(&has_bits);
          queue_head_position_at_reservation_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 96)) {
          _Internal::set_has_queue_head_position_now(&has_bits);
          queue_head_position_now_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CPackageReservationStatus::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_packageid(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_reservation_state(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(3, this->_internal_queue_position(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_total_queue_size(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        5, this->_internal_reservation_country_code(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(6, this->_internal_expired(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(7, this->_internal_time_expires(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(8, this->_internal_time_reserved(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(9, this->_internal_rtime_estimated_notification(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        10, this->_internal_notificaton_token(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(11, this->_internal_queue_head_position_at_reservation(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(12, this->_internal_queue_head_position_now(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CPackageReservationStatus::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_reservation_country_code());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_notificaton_token());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_packageid());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_reservation_state());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_queue_position());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_total_queue_size());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_time_expires());
    }

  }
  if (cached_has_bits & 0x00000f00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_time_reserved());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_rtime_estimated_notification());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_queue_head_position_at_reservation());
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_queue_head_position_now());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CPackageReservationStatus::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CPackageReservationStatus*>(
      &from));
}

void CPackageReservationStatus::MergeFrom(const CPackageReservationStatus& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_reservation_country_code(from._internal_reservation_country_code());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_notificaton_token(from._internal_notificaton_token());
    }
    if (cached_has_bits & 0x00000004u) {
      packageid_ = from.packageid_;
    }
    if (cached_has_bits & 0x00000008u) {
      reservation_state_ = from.reservation_state_;
    }
    if (cached_has_bits & 0x00000010u) {
      queue_position_ = from.queue_position_;
    }
    if (cached_has_bits & 0x00000020u) {
      total_queue_size_ = from.total_queue_size_;
    }
    if (cached_has_bits & 0x00000040u) {
      expired_ = from.expired_;
    }
    if (cached_has_bits & 0x00000080u) {
      time_expires_ = from.time_expires_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00000f00u) {
    if (cached_has_bits & 0x00000100u) {
      time_reserved_ = from.time_reserved_;
    }
    if (cached_has_bits & 0x00000200u) {
      rtime_estimated_notification_ = from.rtime_estimated_notification_;
    }
    if (cached_has_bits & 0x00000400u) {
      queue_head_position_at_reservation_ = from.queue_head_position_at_reservation_;
    }
    if (cached_has_bits & 0x00000800u) {
      queue_head_position_now_ = from.queue_head_position_now_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CPackageReservationStatus::CopyFrom(const CPackageReservationStatus& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CPackageReservationStatus::IsInitialized() const {
  return true;
}

void CPackageReservationStatus::InternalSwap(CPackageReservationStatus* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  reservation_country_code_.Swap(&other->reservation_country_code_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  notificaton_token_.Swap(&other->notificaton_token_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CPackageReservationStatus, queue_head_position_now_)
      + sizeof(CPackageReservationStatus::queue_head_position_now_)
      - PROTOBUF_FIELD_OFFSET(CPackageReservationStatus, packageid_)>(
          reinterpret_cast<char*>(&packageid_),
          reinterpret_cast<char*>(&other->packageid_));
}

std::string CPackageReservationStatus::GetTypeName() const {
  return "CPackageReservationStatus";
}

class CMsgKeyValuePair::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgKeyValuePair>()._has_bits_);
  static void set_has_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_value(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

CMsgKeyValuePair::CMsgKeyValuePair(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgKeyValuePair::CMsgKeyValuePair(const CMsgKeyValuePair& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_name()) {
    name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_name(),
      GetArena());
  }
  value_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_value()) {
    value_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_value(),
      GetArena());
  }

}

void CMsgKeyValuePair::SharedCtor() {
name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
value_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

CMsgKeyValuePair::~CMsgKeyValuePair() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgKeyValuePair::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  value_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgKeyValuePair::ArenaDtor(void* object) {
  CMsgKeyValuePair* _this = reinterpret_cast< CMsgKeyValuePair* >(object);
  (void)_this;
}
void CMsgKeyValuePair::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgKeyValuePair::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgKeyValuePair::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      value_.ClearNonDefaultToEmpty();
    }
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgKeyValuePair::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_value();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgKeyValuePair::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_name(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_value(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgKeyValuePair::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_value());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgKeyValuePair::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgKeyValuePair*>(
      &from));
}

void CMsgKeyValuePair::MergeFrom(const CMsgKeyValuePair& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_name(from._internal_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_value(from._internal_value());
    }
  }
}

void CMsgKeyValuePair::CopyFrom(const CMsgKeyValuePair& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgKeyValuePair::IsInitialized() const {
  return true;
}

void CMsgKeyValuePair::InternalSwap(CMsgKeyValuePair* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  name_.Swap(&other->name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  value_.Swap(&other->value_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

std::string CMsgKeyValuePair::GetTypeName() const {
  return "CMsgKeyValuePair";
}

class CMsgKeyValueSet::_Internal {
 public:
};

CMsgKeyValueSet::CMsgKeyValueSet(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  pairs_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgKeyValueSet::CMsgKeyValueSet(const CMsgKeyValueSet& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      pairs_(from.pairs_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);

}

void CMsgKeyValueSet::SharedCtor() {
}

CMsgKeyValueSet::~CMsgKeyValueSet() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgKeyValueSet::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgKeyValueSet::ArenaDtor(void* object) {
  CMsgKeyValueSet* _this = reinterpret_cast< CMsgKeyValueSet* >(object);
  (void)_this;
}
void CMsgKeyValueSet::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgKeyValueSet::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgKeyValueSet::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  pairs_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgKeyValueSet::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_pairs(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgKeyValueSet::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_pairs_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(1, this->_internal_pairs(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgKeyValueSet::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  total_size += 1UL * this->_internal_pairs_size();
  for (const auto& msg : this->pairs_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgKeyValueSet::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgKeyValueSet*>(
      &from));
}

void CMsgKeyValueSet::MergeFrom(const CMsgKeyValueSet& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  pairs_.MergeFrom(from.pairs_);
}

void CMsgKeyValueSet::CopyFrom(const CMsgKeyValueSet& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgKeyValueSet::IsInitialized() const {
  return true;
}

void CMsgKeyValueSet::InternalSwap(CMsgKeyValueSet* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  pairs_.InternalSwap(&other->pairs_);
}

std::string CMsgKeyValueSet::GetTypeName() const {
  return "CMsgKeyValueSet";
}

class UserContentDescriptorPreferences_ContentDescriptor::_Internal {
 public:
  using HasBits = decltype(std::declval<UserContentDescriptorPreferences_ContentDescriptor>()._has_bits_);
  static void set_has_content_descriptorid(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_timestamp_added(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

UserContentDescriptorPreferences_ContentDescriptor::UserContentDescriptorPreferences_ContentDescriptor(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
UserContentDescriptorPreferences_ContentDescriptor::UserContentDescriptorPreferences_ContentDescriptor(const UserContentDescriptorPreferences_ContentDescriptor& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&content_descriptorid_, &from.content_descriptorid_,
    static_cast<size_t>(reinterpret_cast<char*>(&timestamp_added_) -
    reinterpret_cast<char*>(&content_descriptorid_)) + sizeof(timestamp_added_));

}

void UserContentDescriptorPreferences_ContentDescriptor::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&content_descriptorid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&timestamp_added_) -
    reinterpret_cast<char*>(&content_descriptorid_)) + sizeof(timestamp_added_));
}

UserContentDescriptorPreferences_ContentDescriptor::~UserContentDescriptorPreferences_ContentDescriptor() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void UserContentDescriptorPreferences_ContentDescriptor::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void UserContentDescriptorPreferences_ContentDescriptor::ArenaDtor(void* object) {
  UserContentDescriptorPreferences_ContentDescriptor* _this = reinterpret_cast< UserContentDescriptorPreferences_ContentDescriptor* >(object);
  (void)_this;
}
void UserContentDescriptorPreferences_ContentDescriptor::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void UserContentDescriptorPreferences_ContentDescriptor::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void UserContentDescriptorPreferences_ContentDescriptor::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    ::memset(&content_descriptorid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&timestamp_added_) -
        reinterpret_cast<char*>(&content_descriptorid_)) + sizeof(timestamp_added_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* UserContentDescriptorPreferences_ContentDescriptor::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_content_descriptorid(&has_bits);
          content_descriptorid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_timestamp_added(&has_bits);
          timestamp_added_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* UserContentDescriptorPreferences_ContentDescriptor::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_content_descriptorid(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_timestamp_added(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t UserContentDescriptorPreferences_ContentDescriptor::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_content_descriptorid());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_timestamp_added());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void UserContentDescriptorPreferences_ContentDescriptor::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const UserContentDescriptorPreferences_ContentDescriptor*>(
      &from));
}

void UserContentDescriptorPreferences_ContentDescriptor::MergeFrom(const UserContentDescriptorPreferences_ContentDescriptor& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      content_descriptorid_ = from.content_descriptorid_;
    }
    if (cached_has_bits & 0x00000002u) {
      timestamp_added_ = from.timestamp_added_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void UserContentDescriptorPreferences_ContentDescriptor::CopyFrom(const UserContentDescriptorPreferences_ContentDescriptor& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool UserContentDescriptorPreferences_ContentDescriptor::IsInitialized() const {
  return true;
}

void UserContentDescriptorPreferences_ContentDescriptor::InternalSwap(UserContentDescriptorPreferences_ContentDescriptor* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(UserContentDescriptorPreferences_ContentDescriptor, timestamp_added_)
      + sizeof(UserContentDescriptorPreferences_ContentDescriptor::timestamp_added_)
      - PROTOBUF_FIELD_OFFSET(UserContentDescriptorPreferences_ContentDescriptor, content_descriptorid_)>(
          reinterpret_cast<char*>(&content_descriptorid_),
          reinterpret_cast<char*>(&other->content_descriptorid_));
}

std::string UserContentDescriptorPreferences_ContentDescriptor::GetTypeName() const {
  return "UserContentDescriptorPreferences.ContentDescriptor";
}

class UserContentDescriptorPreferences::_Internal {
 public:
};

UserContentDescriptorPreferences::UserContentDescriptorPreferences(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena),
  content_descriptors_to_exclude_(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
UserContentDescriptorPreferences::UserContentDescriptorPreferences(const UserContentDescriptorPreferences& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      content_descriptors_to_exclude_(from.content_descriptors_to_exclude_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);

}

void UserContentDescriptorPreferences::SharedCtor() {
}

UserContentDescriptorPreferences::~UserContentDescriptorPreferences() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void UserContentDescriptorPreferences::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void UserContentDescriptorPreferences::ArenaDtor(void* object) {
  UserContentDescriptorPreferences* _this = reinterpret_cast< UserContentDescriptorPreferences* >(object);
  (void)_this;
}
void UserContentDescriptorPreferences::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void UserContentDescriptorPreferences::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void UserContentDescriptorPreferences::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  content_descriptors_to_exclude_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* UserContentDescriptorPreferences::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_content_descriptors_to_exclude(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* UserContentDescriptorPreferences::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_content_descriptors_to_exclude_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(1, this->_internal_content_descriptors_to_exclude(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t UserContentDescriptorPreferences::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  total_size += 1UL * this->_internal_content_descriptors_to_exclude_size();
  for (const auto& msg : this->content_descriptors_to_exclude_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void UserContentDescriptorPreferences::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const UserContentDescriptorPreferences*>(
      &from));
}

void UserContentDescriptorPreferences::MergeFrom(const UserContentDescriptorPreferences& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  content_descriptors_to_exclude_.MergeFrom(from.content_descriptors_to_exclude_);
}

void UserContentDescriptorPreferences::CopyFrom(const UserContentDescriptorPreferences& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool UserContentDescriptorPreferences::IsInitialized() const {
  return true;
}

void UserContentDescriptorPreferences::InternalSwap(UserContentDescriptorPreferences* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  content_descriptors_to_exclude_.InternalSwap(&other->content_descriptors_to_exclude_);
}

std::string UserContentDescriptorPreferences::GetTypeName() const {
  return "UserContentDescriptorPreferences";
}

class UserSystemInformation::_Internal {
 public:
  using HasBits = decltype(std::declval<UserSystemInformation>()._has_bits_);
  static void set_has_manufacturer(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_model(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_dx_video_card(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_dx_vendorid(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_dx_deviceid(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_num_gpu(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_system_ram(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_os(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_cpu_vendor(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_cpu_name(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_gaming_device_type(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_dx_driver_version(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_adapter_description(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_driver_version(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_driver_date(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_vram_size(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_screen_width(HasBits* has_bits) {
    (*has_bits)[0] |= 65536u;
  }
  static void set_has_screen_height(HasBits* has_bits) {
    (*has_bits)[0] |= 131072u;
  }
  static void set_has_precise_frame_rate(HasBits* has_bits) {
    (*has_bits)[0] |= 262144u;
  }
};

UserSystemInformation::UserSystemInformation(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
UserSystemInformation::UserSystemInformation(const UserSystemInformation& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  manufacturer_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_manufacturer()) {
    manufacturer_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_manufacturer(),
      GetArena());
  }
  model_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_model()) {
    model_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_model(),
      GetArena());
  }
  dx_video_card_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_dx_video_card()) {
    dx_video_card_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_dx_video_card(),
      GetArena());
  }
  os_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_os()) {
    os_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_os(),
      GetArena());
  }
  cpu_vendor_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_cpu_vendor()) {
    cpu_vendor_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_cpu_vendor(),
      GetArena());
  }
  cpu_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_cpu_name()) {
    cpu_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_cpu_name(),
      GetArena());
  }
  dx_driver_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_dx_driver_version()) {
    dx_driver_version_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_dx_driver_version(),
      GetArena());
  }
  adapter_description_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_adapter_description()) {
    adapter_description_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_adapter_description(),
      GetArena());
  }
  driver_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_driver_version()) {
    driver_version_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_driver_version(),
      GetArena());
  }
  driver_date_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_driver_date()) {
    driver_date_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_driver_date(),
      GetArena());
  }
  ::memcpy(&dx_vendorid_, &from.dx_vendorid_,
    static_cast<size_t>(reinterpret_cast<char*>(&precise_frame_rate_) -
    reinterpret_cast<char*>(&dx_vendorid_)) + sizeof(precise_frame_rate_));

}

void UserSystemInformation::SharedCtor() {
manufacturer_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
model_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
dx_video_card_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
os_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
cpu_vendor_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
cpu_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
dx_driver_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
adapter_description_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
driver_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
driver_date_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&dx_vendorid_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&precise_frame_rate_) -
    reinterpret_cast<char*>(&dx_vendorid_)) + sizeof(precise_frame_rate_));
}

UserSystemInformation::~UserSystemInformation() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void UserSystemInformation::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  manufacturer_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  model_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  dx_video_card_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  os_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  cpu_vendor_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  cpu_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  dx_driver_version_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  adapter_description_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  driver_version_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  driver_date_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void UserSystemInformation::ArenaDtor(void* object) {
  UserSystemInformation* _this = reinterpret_cast< UserSystemInformation* >(object);
  (void)_this;
}
void UserSystemInformation::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void UserSystemInformation::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void UserSystemInformation::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      manufacturer_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      model_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      dx_video_card_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      os_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000010u) {
      cpu_vendor_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000020u) {
      cpu_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000040u) {
      dx_driver_version_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000080u) {
      adapter_description_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x00000300u) {
    if (cached_has_bits & 0x00000100u) {
      driver_version_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000200u) {
      driver_date_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x0000fc00u) {
    ::memset(&dx_vendorid_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&vram_size_) -
        reinterpret_cast<char*>(&dx_vendorid_)) + sizeof(vram_size_));
  }
  if (cached_has_bits & 0x00070000u) {
    ::memset(&screen_width_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&precise_frame_rate_) -
        reinterpret_cast<char*>(&screen_width_)) + sizeof(precise_frame_rate_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* UserSystemInformation::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_manufacturer();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_model();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_dx_video_card();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_dx_vendorid(&has_bits);
          dx_vendorid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_dx_deviceid(&has_bits);
          dx_deviceid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_num_gpu(&has_bits);
          num_gpu_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_system_ram(&has_bits);
          system_ram_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 66)) {
          auto str = _internal_mutable_os();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          auto str = _internal_mutable_cpu_vendor();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          auto str = _internal_mutable_cpu_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 88)) {
          _Internal::set_has_gaming_device_type(&has_bits);
          gaming_device_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 98)) {
          auto str = _internal_mutable_dx_driver_version();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 114)) {
          auto str = _internal_mutable_adapter_description();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 122)) {
          auto str = _internal_mutable_driver_version();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 130)) {
          auto str = _internal_mutable_driver_date();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_vram_size(&has_bits);
          vram_size_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 18:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          _Internal::set_has_screen_width(&has_bits);
          screen_width_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 19:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 152)) {
          _Internal::set_has_screen_height(&has_bits);
          screen_height_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 160)) {
          _Internal::set_has_precise_frame_rate(&has_bits);
          precise_frame_rate_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* UserSystemInformation::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_manufacturer(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_model(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_dx_video_card(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_dx_vendorid(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(5, this->_internal_dx_deviceid(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_num_gpu(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(7, this->_internal_system_ram(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteStringMaybeAliased(
        8, this->_internal_os(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->WriteStringMaybeAliased(
        9, this->_internal_cpu_vendor(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->WriteStringMaybeAliased(
        10, this->_internal_cpu_name(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(11, this->_internal_gaming_device_type(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->WriteStringMaybeAliased(
        12, this->_internal_dx_driver_version(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->WriteStringMaybeAliased(
        14, this->_internal_adapter_description(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->WriteStringMaybeAliased(
        15, this->_internal_driver_version(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->WriteStringMaybeAliased(
        16, this->_internal_driver_date(), target);
  }

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(17, this->_internal_vram_size(), target);
  }

  if (cached_has_bits & 0x00010000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(18, this->_internal_screen_width(), target);
  }

  if (cached_has_bits & 0x00020000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(19, this->_internal_screen_height(), target);
  }

  if (cached_has_bits & 0x00040000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(20, this->_internal_precise_frame_rate(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t UserSystemInformation::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_manufacturer());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_model());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_dx_video_card());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_os());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_cpu_vendor());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_cpu_name());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_dx_driver_version());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_adapter_description());
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_driver_version());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_driver_date());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_dx_vendorid());
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_dx_deviceid());
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_system_ram());
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_num_gpu());
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_gaming_device_type());
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_vram_size());
    }

  }
  if (cached_has_bits & 0x00070000u) {

    if (cached_has_bits & 0x00010000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_screen_width());
    }

    if (cached_has_bits & 0x00020000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_screen_height());
    }

    if (cached_has_bits & 0x00040000u) {
      total_size += 2 + 1;
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void UserSystemInformation::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const UserSystemInformation*>(
      &from));
}

void UserSystemInformation::MergeFrom(const UserSystemInformation& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_manufacturer(from._internal_manufacturer());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_model(from._internal_model());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_dx_video_card(from._internal_dx_video_card());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_os(from._internal_os());
    }
    if (cached_has_bits & 0x00000010u) {
      _internal_set_cpu_vendor(from._internal_cpu_vendor());
    }
    if (cached_has_bits & 0x00000020u) {
      _internal_set_cpu_name(from._internal_cpu_name());
    }
    if (cached_has_bits & 0x00000040u) {
      _internal_set_dx_driver_version(from._internal_dx_driver_version());
    }
    if (cached_has_bits & 0x00000080u) {
      _internal_set_adapter_description(from._internal_adapter_description());
    }
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      _internal_set_driver_version(from._internal_driver_version());
    }
    if (cached_has_bits & 0x00000200u) {
      _internal_set_driver_date(from._internal_driver_date());
    }
    if (cached_has_bits & 0x00000400u) {
      dx_vendorid_ = from.dx_vendorid_;
    }
    if (cached_has_bits & 0x00000800u) {
      dx_deviceid_ = from.dx_deviceid_;
    }
    if (cached_has_bits & 0x00001000u) {
      system_ram_ = from.system_ram_;
    }
    if (cached_has_bits & 0x00002000u) {
      num_gpu_ = from.num_gpu_;
    }
    if (cached_has_bits & 0x00004000u) {
      gaming_device_type_ = from.gaming_device_type_;
    }
    if (cached_has_bits & 0x00008000u) {
      vram_size_ = from.vram_size_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00070000u) {
    if (cached_has_bits & 0x00010000u) {
      screen_width_ = from.screen_width_;
    }
    if (cached_has_bits & 0x00020000u) {
      screen_height_ = from.screen_height_;
    }
    if (cached_has_bits & 0x00040000u) {
      precise_frame_rate_ = from.precise_frame_rate_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void UserSystemInformation::CopyFrom(const UserSystemInformation& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool UserSystemInformation::IsInitialized() const {
  return true;
}

void UserSystemInformation::InternalSwap(UserSystemInformation* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  manufacturer_.Swap(&other->manufacturer_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  model_.Swap(&other->model_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  dx_video_card_.Swap(&other->dx_video_card_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  os_.Swap(&other->os_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  cpu_vendor_.Swap(&other->cpu_vendor_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  cpu_name_.Swap(&other->cpu_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  dx_driver_version_.Swap(&other->dx_driver_version_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  adapter_description_.Swap(&other->adapter_description_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  driver_version_.Swap(&other->driver_version_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  driver_date_.Swap(&other->driver_date_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(UserSystemInformation, precise_frame_rate_)
      + sizeof(UserSystemInformation::precise_frame_rate_)
      - PROTOBUF_FIELD_OFFSET(UserSystemInformation, dx_vendorid_)>(
          reinterpret_cast<char*>(&dx_vendorid_),
          reinterpret_cast<char*>(&other->dx_vendorid_));
}

std::string UserSystemInformation::GetTypeName() const {
  return "UserSystemInformation";
}

class GamePerformanceSettings::_Internal {
 public:
  using HasBits = decltype(std::declval<GamePerformanceSettings>()._has_bits_);
  static void set_has_setting(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_game_resolution_width(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_game_resolution_height(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

GamePerformanceSettings::GamePerformanceSettings(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
GamePerformanceSettings::GamePerformanceSettings(const GamePerformanceSettings& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&setting_, &from.setting_,
    static_cast<size_t>(reinterpret_cast<char*>(&game_resolution_height_) -
    reinterpret_cast<char*>(&setting_)) + sizeof(game_resolution_height_));

}

void GamePerformanceSettings::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&setting_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&game_resolution_height_) -
    reinterpret_cast<char*>(&setting_)) + sizeof(game_resolution_height_));
}

GamePerformanceSettings::~GamePerformanceSettings() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void GamePerformanceSettings::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void GamePerformanceSettings::ArenaDtor(void* object) {
  GamePerformanceSettings* _this = reinterpret_cast< GamePerformanceSettings* >(object);
  (void)_this;
}
void GamePerformanceSettings::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void GamePerformanceSettings::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void GamePerformanceSettings::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    ::memset(&setting_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&game_resolution_height_) -
        reinterpret_cast<char*>(&setting_)) + sizeof(game_resolution_height_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* GamePerformanceSettings::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::GamePerformanceSettings_EGamePerformanceSetting_IsValid(val))) {
            _internal_set_setting(static_cast<::GamePerformanceSettings_EGamePerformanceSetting>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(1, val, mutable_unknown_fields());
          }
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_game_resolution_width(&has_bits);
          game_resolution_width_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_game_resolution_height(&has_bits);
          game_resolution_height_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<std::string>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }
  }
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* GamePerformanceSettings::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_setting(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_game_resolution_width(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_game_resolution_height(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t GamePerformanceSettings::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_setting());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_game_resolution_width());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_game_resolution_height());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void GamePerformanceSettings::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const GamePerformanceSettings*>(
      &from));
}

void GamePerformanceSettings::MergeFrom(const GamePerformanceSettings& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      setting_ = from.setting_;
    }
    if (cached_has_bits & 0x00000002u) {
      game_resolution_width_ = from.game_resolution_width_;
    }
    if (cached_has_bits & 0x00000004u) {
      game_resolution_height_ = from.game_resolution_height_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void GamePerformanceSettings::CopyFrom(const GamePerformanceSettings& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GamePerformanceSettings::IsInitialized() const {
  return true;
}

void GamePerformanceSettings::InternalSwap(GamePerformanceSettings* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(GamePerformanceSettings, game_resolution_height_)
      + sizeof(GamePerformanceSettings::game_resolution_height_)
      - PROTOBUF_FIELD_OFFSET(GamePerformanceSettings, setting_)>(
          reinterpret_cast<char*>(&setting_),
          reinterpret_cast<char*>(&other->setting_));
}

std::string GamePerformanceSettings::GetTypeName() const {
  return "GamePerformanceSettings";
}

PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::CMsgIPAddress* Arena::CreateMaybeMessage< ::CMsgIPAddress >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgIPAddress >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgIPAddressBucket* Arena::CreateMaybeMessage< ::CMsgIPAddressBucket >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgIPAddressBucket >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgGCRoutingProtoBufHeader* Arena::CreateMaybeMessage< ::CMsgGCRoutingProtoBufHeader >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgGCRoutingProtoBufHeader >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgProtoBufHeader* Arena::CreateMaybeMessage< ::CMsgProtoBufHeader >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgProtoBufHeader >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgKubeRPCPacket_Hdr* Arena::CreateMaybeMessage< ::CMsgKubeRPCPacket_Hdr >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgKubeRPCPacket_Hdr >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgKubeRPCPacket* Arena::CreateMaybeMessage< ::CMsgKubeRPCPacket >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgKubeRPCPacket >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgMulti* Arena::CreateMaybeMessage< ::CMsgMulti >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgMulti >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgProtobufWrapped* Arena::CreateMaybeMessage< ::CMsgProtobufWrapped >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgProtobufWrapped >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgAuthTicket* Arena::CreateMaybeMessage< ::CMsgAuthTicket >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgAuthTicket >(arena);
}
template<> PROTOBUF_NOINLINE ::CCDDBAppDetailCommon* Arena::CreateMaybeMessage< ::CCDDBAppDetailCommon >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CCDDBAppDetailCommon >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgAppRights* Arena::CreateMaybeMessage< ::CMsgAppRights >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgAppRights >(arena);
}
template<> PROTOBUF_NOINLINE ::CCuratorPreferences* Arena::CreateMaybeMessage< ::CCuratorPreferences >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CCuratorPreferences >(arena);
}
template<> PROTOBUF_NOINLINE ::CLocalizationToken* Arena::CreateMaybeMessage< ::CLocalizationToken >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CLocalizationToken >(arena);
}
template<> PROTOBUF_NOINLINE ::CClanEventUserNewsTuple* Arena::CreateMaybeMessage< ::CClanEventUserNewsTuple >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CClanEventUserNewsTuple >(arena);
}
template<> PROTOBUF_NOINLINE ::CClanMatchEventByRange* Arena::CreateMaybeMessage< ::CClanMatchEventByRange >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CClanMatchEventByRange >(arena);
}
template<> PROTOBUF_NOINLINE ::CCommunity_ClanAnnouncementInfo* Arena::CreateMaybeMessage< ::CCommunity_ClanAnnouncementInfo >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CCommunity_ClanAnnouncementInfo >(arena);
}
template<> PROTOBUF_NOINLINE ::CClanEventData* Arena::CreateMaybeMessage< ::CClanEventData >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CClanEventData >(arena);
}
template<> PROTOBUF_NOINLINE ::CBilling_Address* Arena::CreateMaybeMessage< ::CBilling_Address >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CBilling_Address >(arena);
}
template<> PROTOBUF_NOINLINE ::CPackageReservationStatus* Arena::CreateMaybeMessage< ::CPackageReservationStatus >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CPackageReservationStatus >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgKeyValuePair* Arena::CreateMaybeMessage< ::CMsgKeyValuePair >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgKeyValuePair >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgKeyValueSet* Arena::CreateMaybeMessage< ::CMsgKeyValueSet >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgKeyValueSet >(arena);
}
template<> PROTOBUF_NOINLINE ::UserContentDescriptorPreferences_ContentDescriptor* Arena::CreateMaybeMessage< ::UserContentDescriptorPreferences_ContentDescriptor >(Arena* arena) {
  return Arena::CreateMessageInternal< ::UserContentDescriptorPreferences_ContentDescriptor >(arena);
}
template<> PROTOBUF_NOINLINE ::UserContentDescriptorPreferences* Arena::CreateMaybeMessage< ::UserContentDescriptorPreferences >(Arena* arena) {
  return Arena::CreateMessageInternal< ::UserContentDescriptorPreferences >(arena);
}
template<> PROTOBUF_NOINLINE ::UserSystemInformation* Arena::CreateMaybeMessage< ::UserSystemInformation >(Arena* arena) {
  return Arena::CreateMessageInternal< ::UserSystemInformation >(arena);
}
template<> PROTOBUF_NOINLINE ::GamePerformanceSettings* Arena::CreateMaybeMessage< ::GamePerformanceSettings >(Arena* arena) {
  return Arena::CreateMessageInternal< ::GamePerformanceSettings >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

#include <google/protobuf/port_undef.inc>
