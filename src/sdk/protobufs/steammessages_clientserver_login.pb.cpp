

#include "steammessages_clientserver_login.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
constexpr CMsgClientHeartBeat::CMsgClientHeartBeat(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : send_reply_(false){}
struct CMsgClientHeartBeatDefaultTypeInternal {
  constexpr CMsgClientHeartBeatDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientHeartBeatDefaultTypeInternal() {}
  union {
    CMsgClientHeartBeat _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientHeartBeatDefaultTypeInternal _CMsgClientHeartBeat_default_instance_;
constexpr CMsgClientServerTimestampRequest::CMsgClientServerTimestampRequest(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : client_request_timestamp_(PROTOBUF_ULONGLONG(0)){}
struct CMsgClientServerTimestampRequestDefaultTypeInternal {
  constexpr CMsgClientServerTimestampRequestDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientServerTimestampRequestDefaultTypeInternal() {}
  union {
    CMsgClientServerTimestampRequest _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientServerTimestampRequestDefaultTypeInternal _CMsgClientServerTimestampRequest_default_instance_;
constexpr CMsgClientServerTimestampResponse::CMsgClientServerTimestampResponse(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : client_request_timestamp_(PROTOBUF_ULONGLONG(0))
  , server_timestamp_ms_(PROTOBUF_ULONGLONG(0)){}
struct CMsgClientServerTimestampResponseDefaultTypeInternal {
  constexpr CMsgClientServerTimestampResponseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientServerTimestampResponseDefaultTypeInternal() {}
  union {
    CMsgClientServerTimestampResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientServerTimestampResponseDefaultTypeInternal _CMsgClientServerTimestampResponse_default_instance_;
constexpr CMsgClientSecret::CMsgClientSecret(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : hmac_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , version_(0u)
  , appid_(0u)
  , nonce_(PROTOBUF_ULONGLONG(0))
  , deviceid_(0u){}
struct CMsgClientSecretDefaultTypeInternal {
  constexpr CMsgClientSecretDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientSecretDefaultTypeInternal() {}
  union {
    CMsgClientSecret _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientSecretDefaultTypeInternal _CMsgClientSecret_default_instance_;
constexpr CMsgClientHello::CMsgClientHello(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : protocol_version_(0u){}
struct CMsgClientHelloDefaultTypeInternal {
  constexpr CMsgClientHelloDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientHelloDefaultTypeInternal() {}
  union {
    CMsgClientHello _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientHelloDefaultTypeInternal _CMsgClientHello_default_instance_;
constexpr CMsgClientLogon::CMsgClientLogon(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : client_language_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , wine_version_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , machine_id_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , steam2_auth_ticket_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , email_address_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , account_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , password_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , game_server_token_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , login_key_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , anon_user_target_account_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , sha_sentryfile_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , auth_code_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , otp_identifier_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , sony_psn_ticket_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , sony_psn_service_id_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , sony_psn_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , machine_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , machine_name_userchosen_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , country_override_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , two_factor_code_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , web_logon_nonce_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , access_token_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , obfuscated_private_ip_(nullptr)
  , public_ip_(nullptr)
  , embedded_client_secret_(nullptr)
  , protocol_version_(0u)
  , deprecated_obfustucated_private_ip_(0u)
  , cell_id_(0u)
  , last_session_id_(0u)
  , client_package_version_(0u)
  , client_os_type_(0u)
  , deprecated_10_(0u)
  , deprecated_public_ip_(0u)
  , client_supplied_steam_id_(PROTOBUF_ULONGLONG(0))
  , qos_level_(0u)
  , launcher_type_(0u)
  , ui_mode_(0u)
  , chat_mode_(0u)
  , rtime32_account_creation_(0u)
  , should_remember_password_(false)
  , was_converted_deprecated_msg_(false)
  , steam2_ticket_request_(false)
  , create_new_psn_linked_account_if_needed_(false)
  , resolved_user_steam_id_(PROTOBUF_ULONGLONG(0))
  , eresult_sentryfile_(0)
  , otp_type_(0)
  , otp_value_(0u)
  , game_server_app_id_(0)
  , client_instance_id_(PROTOBUF_ULONGLONG(0))
  , steamguard_dont_remember_computer_(false)
  , supports_rate_limit_response_(false)
  , disable_partner_autogrants_(false)
  , is_chrome_os_(false)
  , priority_reason_(0)
  , is_steam_box_deprecated_(false)
  , is_steam_deck_deprecated_(false)
  , is_tesla_deprecated_(false)
  , gaming_device_type_(0u){}
struct CMsgClientLogonDefaultTypeInternal {
  constexpr CMsgClientLogonDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientLogonDefaultTypeInternal() {}
  union {
    CMsgClientLogon _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientLogonDefaultTypeInternal _CMsgClientLogon_default_instance_;
constexpr CMsgClientLogonResponse::CMsgClientLogonResponse(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : email_domain_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , steam2_ticket_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , vanity_url_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , user_country_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , ip_country_code_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , parental_settings_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , parental_setting_signature_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , agreement_session_url_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , public_ip_(nullptr)
  , legacy_out_of_game_heartbeat_seconds_(0)
  , heartbeat_seconds_(0)
  , deprecated_public_ip_(0u)
  , rtime32_server_time_(0u)
  , account_flags_(0u)
  , cell_id_(0u)
  , eresult_extended_(0)
  , cell_id_ping_threshold_(0u)
  , client_supplied_steamid_(PROTOBUF_ULONGLONG(0))
  , deprecated_use_pics_(false)
  , force_client_update_check_(false)
  , count_loginfailures_to_migrate_(0)
  , count_disconnects_to_migrate_(0)
  , ogs_data_report_time_window_(0)
  , client_instance_id_(PROTOBUF_ULONGLONG(0))
  , token_id_(PROTOBUF_ULONGLONG(0))
  , family_group_id_(PROTOBUF_ULONGLONG(0))
  , eresult_(2){}
struct CMsgClientLogonResponseDefaultTypeInternal {
  constexpr CMsgClientLogonResponseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientLogonResponseDefaultTypeInternal() {}
  union {
    CMsgClientLogonResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientLogonResponseDefaultTypeInternal _CMsgClientLogonResponse_default_instance_;
constexpr CMsgClientRequestWebAPIAuthenticateUserNonce::CMsgClientRequestWebAPIAuthenticateUserNonce(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : token_type_(-1){}
struct CMsgClientRequestWebAPIAuthenticateUserNonceDefaultTypeInternal {
  constexpr CMsgClientRequestWebAPIAuthenticateUserNonceDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientRequestWebAPIAuthenticateUserNonceDefaultTypeInternal() {}
  union {
    CMsgClientRequestWebAPIAuthenticateUserNonce _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientRequestWebAPIAuthenticateUserNonceDefaultTypeInternal _CMsgClientRequestWebAPIAuthenticateUserNonce_default_instance_;
constexpr CMsgClientRequestWebAPIAuthenticateUserNonceResponse::CMsgClientRequestWebAPIAuthenticateUserNonceResponse(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : webapi_authenticate_user_nonce_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , eresult_(2)
  , token_type_(-1){}
struct CMsgClientRequestWebAPIAuthenticateUserNonceResponseDefaultTypeInternal {
  constexpr CMsgClientRequestWebAPIAuthenticateUserNonceResponseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientRequestWebAPIAuthenticateUserNonceResponseDefaultTypeInternal() {}
  union {
    CMsgClientRequestWebAPIAuthenticateUserNonceResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientRequestWebAPIAuthenticateUserNonceResponseDefaultTypeInternal _CMsgClientRequestWebAPIAuthenticateUserNonceResponse_default_instance_;
constexpr CMsgClientLogOff::CMsgClientLogOff(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized){}
struct CMsgClientLogOffDefaultTypeInternal {
  constexpr CMsgClientLogOffDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientLogOffDefaultTypeInternal() {}
  union {
    CMsgClientLogOff _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientLogOffDefaultTypeInternal _CMsgClientLogOff_default_instance_;
constexpr CMsgClientLoggedOff::CMsgClientLoggedOff(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : eresult_(2){}
struct CMsgClientLoggedOffDefaultTypeInternal {
  constexpr CMsgClientLoggedOffDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientLoggedOffDefaultTypeInternal() {}
  union {
    CMsgClientLoggedOff _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientLoggedOffDefaultTypeInternal _CMsgClientLoggedOff_default_instance_;
constexpr CMsgClientNewLoginKey::CMsgClientNewLoginKey(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : login_key_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , unique_id_(0u){}
struct CMsgClientNewLoginKeyDefaultTypeInternal {
  constexpr CMsgClientNewLoginKeyDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientNewLoginKeyDefaultTypeInternal() {}
  union {
    CMsgClientNewLoginKey _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientNewLoginKeyDefaultTypeInternal _CMsgClientNewLoginKey_default_instance_;
constexpr CMsgClientNewLoginKeyAccepted::CMsgClientNewLoginKeyAccepted(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : unique_id_(0u){}
struct CMsgClientNewLoginKeyAcceptedDefaultTypeInternal {
  constexpr CMsgClientNewLoginKeyAcceptedDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientNewLoginKeyAcceptedDefaultTypeInternal() {}
  union {
    CMsgClientNewLoginKeyAccepted _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientNewLoginKeyAcceptedDefaultTypeInternal _CMsgClientNewLoginKeyAccepted_default_instance_;
constexpr CMsgClientAccountInfo::CMsgClientAccountInfo(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : persona_name_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , ip_country_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , steamguard_machine_name_user_chosen_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , count_authed_computers_(0)
  , account_flags_(0u)
  , two_factor_state_(0u)
  , is_phone_verified_(false)
  , is_phone_identifying_(false)
  , is_phone_needing_reverify_(false){}
struct CMsgClientAccountInfoDefaultTypeInternal {
  constexpr CMsgClientAccountInfoDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientAccountInfoDefaultTypeInternal() {}
  union {
    CMsgClientAccountInfo _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientAccountInfoDefaultTypeInternal _CMsgClientAccountInfo_default_instance_;
constexpr CMsgClientChallengeRequest::CMsgClientChallengeRequest(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : steamid_(PROTOBUF_ULONGLONG(0)){}
struct CMsgClientChallengeRequestDefaultTypeInternal {
  constexpr CMsgClientChallengeRequestDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientChallengeRequestDefaultTypeInternal() {}
  union {
    CMsgClientChallengeRequest _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientChallengeRequestDefaultTypeInternal _CMsgClientChallengeRequest_default_instance_;
constexpr CMsgClientChallengeResponse::CMsgClientChallengeResponse(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : challenge_(PROTOBUF_ULONGLONG(0)){}
struct CMsgClientChallengeResponseDefaultTypeInternal {
  constexpr CMsgClientChallengeResponseDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~CMsgClientChallengeResponseDefaultTypeInternal() {}
  union {
    CMsgClientChallengeResponse _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT CMsgClientChallengeResponseDefaultTypeInternal _CMsgClientChallengeResponse_default_instance_;

class CMsgClientHeartBeat::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientHeartBeat>()._has_bits_);
  static void set_has_send_reply(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientHeartBeat::CMsgClientHeartBeat(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientHeartBeat::CMsgClientHeartBeat(const CMsgClientHeartBeat& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  send_reply_ = from.send_reply_;

}

void CMsgClientHeartBeat::SharedCtor() {
send_reply_ = false;
}

CMsgClientHeartBeat::~CMsgClientHeartBeat() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientHeartBeat::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientHeartBeat::ArenaDtor(void* object) {
  CMsgClientHeartBeat* _this = reinterpret_cast< CMsgClientHeartBeat* >(object);
  (void)_this;
}
void CMsgClientHeartBeat::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientHeartBeat::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientHeartBeat::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  send_reply_ = false;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientHeartBeat::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_send_reply(&has_bits);
          send_reply_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientHeartBeat::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(1, this->_internal_send_reply(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientHeartBeat::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 + 1;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientHeartBeat::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientHeartBeat*>(
      &from));
}

void CMsgClientHeartBeat::MergeFrom(const CMsgClientHeartBeat& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_send_reply()) {
    _internal_set_send_reply(from._internal_send_reply());
  }
}

void CMsgClientHeartBeat::CopyFrom(const CMsgClientHeartBeat& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientHeartBeat::IsInitialized() const {
  return true;
}

void CMsgClientHeartBeat::InternalSwap(CMsgClientHeartBeat* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(send_reply_, other->send_reply_);
}

std::string CMsgClientHeartBeat::GetTypeName() const {
  return "CMsgClientHeartBeat";
}

class CMsgClientServerTimestampRequest::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientServerTimestampRequest>()._has_bits_);
  static void set_has_client_request_timestamp(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientServerTimestampRequest::CMsgClientServerTimestampRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientServerTimestampRequest::CMsgClientServerTimestampRequest(const CMsgClientServerTimestampRequest& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  client_request_timestamp_ = from.client_request_timestamp_;

}

void CMsgClientServerTimestampRequest::SharedCtor() {
client_request_timestamp_ = PROTOBUF_ULONGLONG(0);
}

CMsgClientServerTimestampRequest::~CMsgClientServerTimestampRequest() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientServerTimestampRequest::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientServerTimestampRequest::ArenaDtor(void* object) {
  CMsgClientServerTimestampRequest* _this = reinterpret_cast< CMsgClientServerTimestampRequest* >(object);
  (void)_this;
}
void CMsgClientServerTimestampRequest::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientServerTimestampRequest::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientServerTimestampRequest::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  client_request_timestamp_ = PROTOBUF_ULONGLONG(0);
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientServerTimestampRequest::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_client_request_timestamp(&has_bits);
          client_request_timestamp_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientServerTimestampRequest::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(1, this->_internal_client_request_timestamp(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientServerTimestampRequest::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_client_request_timestamp());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientServerTimestampRequest::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientServerTimestampRequest*>(
      &from));
}

void CMsgClientServerTimestampRequest::MergeFrom(const CMsgClientServerTimestampRequest& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_client_request_timestamp()) {
    _internal_set_client_request_timestamp(from._internal_client_request_timestamp());
  }
}

void CMsgClientServerTimestampRequest::CopyFrom(const CMsgClientServerTimestampRequest& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientServerTimestampRequest::IsInitialized() const {
  return true;
}

void CMsgClientServerTimestampRequest::InternalSwap(CMsgClientServerTimestampRequest* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(client_request_timestamp_, other->client_request_timestamp_);
}

std::string CMsgClientServerTimestampRequest::GetTypeName() const {
  return "CMsgClientServerTimestampRequest";
}

class CMsgClientServerTimestampResponse::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientServerTimestampResponse>()._has_bits_);
  static void set_has_client_request_timestamp(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_server_timestamp_ms(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

CMsgClientServerTimestampResponse::CMsgClientServerTimestampResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientServerTimestampResponse::CMsgClientServerTimestampResponse(const CMsgClientServerTimestampResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::memcpy(&client_request_timestamp_, &from.client_request_timestamp_,
    static_cast<size_t>(reinterpret_cast<char*>(&server_timestamp_ms_) -
    reinterpret_cast<char*>(&client_request_timestamp_)) + sizeof(server_timestamp_ms_));

}

void CMsgClientServerTimestampResponse::SharedCtor() {
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&client_request_timestamp_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&server_timestamp_ms_) -
    reinterpret_cast<char*>(&client_request_timestamp_)) + sizeof(server_timestamp_ms_));
}

CMsgClientServerTimestampResponse::~CMsgClientServerTimestampResponse() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientServerTimestampResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientServerTimestampResponse::ArenaDtor(void* object) {
  CMsgClientServerTimestampResponse* _this = reinterpret_cast< CMsgClientServerTimestampResponse* >(object);
  (void)_this;
}
void CMsgClientServerTimestampResponse::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientServerTimestampResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientServerTimestampResponse::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    ::memset(&client_request_timestamp_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&server_timestamp_ms_) -
        reinterpret_cast<char*>(&client_request_timestamp_)) + sizeof(server_timestamp_ms_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientServerTimestampResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_client_request_timestamp(&has_bits);
          client_request_timestamp_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_server_timestamp_ms(&has_bits);
          server_timestamp_ms_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientServerTimestampResponse::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(1, this->_internal_client_request_timestamp(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(2, this->_internal_server_timestamp_ms(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientServerTimestampResponse::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_client_request_timestamp());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_server_timestamp_ms());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientServerTimestampResponse::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientServerTimestampResponse*>(
      &from));
}

void CMsgClientServerTimestampResponse::MergeFrom(const CMsgClientServerTimestampResponse& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      client_request_timestamp_ = from.client_request_timestamp_;
    }
    if (cached_has_bits & 0x00000002u) {
      server_timestamp_ms_ = from.server_timestamp_ms_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgClientServerTimestampResponse::CopyFrom(const CMsgClientServerTimestampResponse& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientServerTimestampResponse::IsInitialized() const {
  return true;
}

void CMsgClientServerTimestampResponse::InternalSwap(CMsgClientServerTimestampResponse* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgClientServerTimestampResponse, server_timestamp_ms_)
      + sizeof(CMsgClientServerTimestampResponse::server_timestamp_ms_)
      - PROTOBUF_FIELD_OFFSET(CMsgClientServerTimestampResponse, client_request_timestamp_)>(
          reinterpret_cast<char*>(&client_request_timestamp_),
          reinterpret_cast<char*>(&other->client_request_timestamp_));
}

std::string CMsgClientServerTimestampResponse::GetTypeName() const {
  return "CMsgClientServerTimestampResponse";
}

class CMsgClientSecret::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientSecret>()._has_bits_);
  static void set_has_version(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_appid(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_deviceid(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_nonce(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_hmac(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientSecret::CMsgClientSecret(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientSecret::CMsgClientSecret(const CMsgClientSecret& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  hmac_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_hmac()) {
    hmac_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_hmac(),
      GetArena());
  }
  ::memcpy(&version_, &from.version_,
    static_cast<size_t>(reinterpret_cast<char*>(&deviceid_) -
    reinterpret_cast<char*>(&version_)) + sizeof(deviceid_));

}

void CMsgClientSecret::SharedCtor() {
hmac_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&version_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&deviceid_) -
    reinterpret_cast<char*>(&version_)) + sizeof(deviceid_));
}

CMsgClientSecret::~CMsgClientSecret() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientSecret::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  hmac_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgClientSecret::ArenaDtor(void* object) {
  CMsgClientSecret* _this = reinterpret_cast< CMsgClientSecret* >(object);
  (void)_this;
}
void CMsgClientSecret::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientSecret::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientSecret::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    hmac_.ClearNonDefaultToEmpty();
  }
  if (cached_has_bits & 0x0000001eu) {
    ::memset(&version_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&deviceid_) -
        reinterpret_cast<char*>(&version_)) + sizeof(deviceid_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientSecret::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_version(&has_bits);
          version_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_appid(&has_bits);
          appid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_deviceid(&has_bits);
          deviceid_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 33)) {
          _Internal::set_has_nonce(&has_bits);
          nonce_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          auto str = _internal_mutable_hmac();
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientSecret::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_version(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_appid(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_deviceid(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(4, this->_internal_nonce(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        5, this->_internal_hmac(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientSecret::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_hmac());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_version());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_appid());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 + 8;
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_deviceid());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientSecret::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientSecret*>(
      &from));
}

void CMsgClientSecret::MergeFrom(const CMsgClientSecret& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_hmac(from._internal_hmac());
    }
    if (cached_has_bits & 0x00000002u) {
      version_ = from.version_;
    }
    if (cached_has_bits & 0x00000004u) {
      appid_ = from.appid_;
    }
    if (cached_has_bits & 0x00000008u) {
      nonce_ = from.nonce_;
    }
    if (cached_has_bits & 0x00000010u) {
      deviceid_ = from.deviceid_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgClientSecret::CopyFrom(const CMsgClientSecret& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientSecret::IsInitialized() const {
  return true;
}

void CMsgClientSecret::InternalSwap(CMsgClientSecret* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  hmac_.Swap(&other->hmac_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgClientSecret, deviceid_)
      + sizeof(CMsgClientSecret::deviceid_)
      - PROTOBUF_FIELD_OFFSET(CMsgClientSecret, version_)>(
          reinterpret_cast<char*>(&version_),
          reinterpret_cast<char*>(&other->version_));
}

std::string CMsgClientSecret::GetTypeName() const {
  return "CMsgClientSecret";
}

class CMsgClientHello::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientHello>()._has_bits_);
  static void set_has_protocol_version(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientHello::CMsgClientHello(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientHello::CMsgClientHello(const CMsgClientHello& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  protocol_version_ = from.protocol_version_;

}

void CMsgClientHello::SharedCtor() {
protocol_version_ = 0u;
}

CMsgClientHello::~CMsgClientHello() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientHello::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientHello::ArenaDtor(void* object) {
  CMsgClientHello* _this = reinterpret_cast< CMsgClientHello* >(object);
  (void)_this;
}
void CMsgClientHello::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientHello::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientHello::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  protocol_version_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientHello::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_protocol_version(&has_bits);
          protocol_version_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientHello::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_protocol_version(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientHello::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_protocol_version());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientHello::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientHello*>(
      &from));
}

void CMsgClientHello::MergeFrom(const CMsgClientHello& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_protocol_version()) {
    _internal_set_protocol_version(from._internal_protocol_version());
  }
}

void CMsgClientHello::CopyFrom(const CMsgClientHello& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientHello::IsInitialized() const {
  return true;
}

void CMsgClientHello::InternalSwap(CMsgClientHello* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(protocol_version_, other->protocol_version_);
}

std::string CMsgClientHello::GetTypeName() const {
  return "CMsgClientHello";
}

class CMsgClientLogon::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientLogon>()._has_bits_);
  static void set_has_protocol_version(HasBits* has_bits) {
    (*has_bits)[0] |= 33554432u;
  }
  static void set_has_deprecated_obfustucated_private_ip(HasBits* has_bits) {
    (*has_bits)[0] |= 67108864u;
  }
  static void set_has_cell_id(HasBits* has_bits) {
    (*has_bits)[0] |= 134217728u;
  }
  static void set_has_last_session_id(HasBits* has_bits) {
    (*has_bits)[0] |= 268435456u;
  }
  static void set_has_client_package_version(HasBits* has_bits) {
    (*has_bits)[0] |= 536870912u;
  }
  static void set_has_client_language(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_client_os_type(HasBits* has_bits) {
    (*has_bits)[0] |= 1073741824u;
  }
  static void set_has_should_remember_password(HasBits* has_bits) {
    (*has_bits)[1] |= 128u;
  }
  static void set_has_wine_version(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_deprecated_10(HasBits* has_bits) {
    (*has_bits)[0] |= 2147483648u;
  }
  static const ::CMsgIPAddress& obfuscated_private_ip(const CMsgClientLogon* msg);
  static void set_has_obfuscated_private_ip(HasBits* has_bits) {
    (*has_bits)[0] |= 4194304u;
  }
  static void set_has_deprecated_public_ip(HasBits* has_bits) {
    (*has_bits)[1] |= 1u;
  }
  static void set_has_qos_level(HasBits* has_bits) {
    (*has_bits)[1] |= 4u;
  }
  static void set_has_client_supplied_steam_id(HasBits* has_bits) {
    (*has_bits)[1] |= 2u;
  }
  static const ::CMsgIPAddress& public_ip(const CMsgClientLogon* msg);
  static void set_has_public_ip(HasBits* has_bits) {
    (*has_bits)[0] |= 8388608u;
  }
  static void set_has_machine_id(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_launcher_type(HasBits* has_bits) {
    (*has_bits)[1] |= 8u;
  }
  static void set_has_ui_mode(HasBits* has_bits) {
    (*has_bits)[1] |= 16u;
  }
  static void set_has_chat_mode(HasBits* has_bits) {
    (*has_bits)[1] |= 32u;
  }
  static void set_has_steam2_auth_ticket(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_email_address(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_rtime32_account_creation(HasBits* has_bits) {
    (*has_bits)[1] |= 64u;
  }
  static void set_has_account_name(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_password(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_game_server_token(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_login_key(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_was_converted_deprecated_msg(HasBits* has_bits) {
    (*has_bits)[1] |= 256u;
  }
  static void set_has_anon_user_target_account_name(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_resolved_user_steam_id(HasBits* has_bits) {
    (*has_bits)[1] |= 2048u;
  }
  static void set_has_eresult_sentryfile(HasBits* has_bits) {
    (*has_bits)[1] |= 4096u;
  }
  static void set_has_sha_sentryfile(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_auth_code(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_otp_type(HasBits* has_bits) {
    (*has_bits)[1] |= 8192u;
  }
  static void set_has_otp_value(HasBits* has_bits) {
    (*has_bits)[1] |= 16384u;
  }
  static void set_has_otp_identifier(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_steam2_ticket_request(HasBits* has_bits) {
    (*has_bits)[1] |= 512u;
  }
  static void set_has_sony_psn_ticket(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_sony_psn_service_id(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_create_new_psn_linked_account_if_needed(HasBits* has_bits) {
    (*has_bits)[1] |= 1024u;
  }
  static void set_has_sony_psn_name(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_game_server_app_id(HasBits* has_bits) {
    (*has_bits)[1] |= 32768u;
  }
  static void set_has_steamguard_dont_remember_computer(HasBits* has_bits) {
    (*has_bits)[1] |= 131072u;
  }
  static void set_has_machine_name(HasBits* has_bits) {
    (*has_bits)[0] |= 65536u;
  }
  static void set_has_machine_name_userchosen(HasBits* has_bits) {
    (*has_bits)[0] |= 131072u;
  }
  static void set_has_country_override(HasBits* has_bits) {
    (*has_bits)[0] |= 262144u;
  }
  static void set_has_client_instance_id(HasBits* has_bits) {
    (*has_bits)[1] |= 65536u;
  }
  static void set_has_two_factor_code(HasBits* has_bits) {
    (*has_bits)[0] |= 524288u;
  }
  static void set_has_supports_rate_limit_response(HasBits* has_bits) {
    (*has_bits)[1] |= 262144u;
  }
  static void set_has_web_logon_nonce(HasBits* has_bits) {
    (*has_bits)[0] |= 1048576u;
  }
  static void set_has_priority_reason(HasBits* has_bits) {
    (*has_bits)[1] |= 2097152u;
  }
  static const ::CMsgClientSecret& embedded_client_secret(const CMsgClientLogon* msg);
  static void set_has_embedded_client_secret(HasBits* has_bits) {
    (*has_bits)[0] |= 16777216u;
  }
  static void set_has_disable_partner_autogrants(HasBits* has_bits) {
    (*has_bits)[1] |= 524288u;
  }
  static void set_has_access_token(HasBits* has_bits) {
    (*has_bits)[0] |= 2097152u;
  }
  static void set_has_is_chrome_os(HasBits* has_bits) {
    (*has_bits)[1] |= 1048576u;
  }
  static void set_has_is_steam_box_deprecated(HasBits* has_bits) {
    (*has_bits)[1] |= 4194304u;
  }
  static void set_has_is_steam_deck_deprecated(HasBits* has_bits) {
    (*has_bits)[1] |= 8388608u;
  }
  static void set_has_is_tesla_deprecated(HasBits* has_bits) {
    (*has_bits)[1] |= 16777216u;
  }
  static void set_has_gaming_device_type(HasBits* has_bits) {
    (*has_bits)[1] |= 33554432u;
  }
};

const ::CMsgIPAddress&
CMsgClientLogon::_Internal::obfuscated_private_ip(const CMsgClientLogon* msg) {
  return *msg->obfuscated_private_ip_;
}
const ::CMsgIPAddress&
CMsgClientLogon::_Internal::public_ip(const CMsgClientLogon* msg) {
  return *msg->public_ip_;
}
const ::CMsgClientSecret&
CMsgClientLogon::_Internal::embedded_client_secret(const CMsgClientLogon* msg) {
  return *msg->embedded_client_secret_;
}
void CMsgClientLogon::clear_obfuscated_private_ip() {
  if (obfuscated_private_ip_ != nullptr) obfuscated_private_ip_->Clear();
  _has_bits_[0] &= ~0x00400000u;
}
void CMsgClientLogon::clear_public_ip() {
  if (public_ip_ != nullptr) public_ip_->Clear();
  _has_bits_[0] &= ~0x00800000u;
}
CMsgClientLogon::CMsgClientLogon(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientLogon::CMsgClientLogon(const CMsgClientLogon& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  client_language_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_client_language()) {
    client_language_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_client_language(),
      GetArena());
  }
  wine_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_wine_version()) {
    wine_version_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_wine_version(),
      GetArena());
  }
  machine_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_machine_id()) {
    machine_id_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_machine_id(),
      GetArena());
  }
  steam2_auth_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_steam2_auth_ticket()) {
    steam2_auth_ticket_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_steam2_auth_ticket(),
      GetArena());
  }
  email_address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_email_address()) {
    email_address_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_email_address(),
      GetArena());
  }
  account_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_account_name()) {
    account_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_account_name(),
      GetArena());
  }
  password_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_password()) {
    password_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_password(),
      GetArena());
  }
  game_server_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_game_server_token()) {
    game_server_token_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_game_server_token(),
      GetArena());
  }
  login_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_login_key()) {
    login_key_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_login_key(),
      GetArena());
  }
  anon_user_target_account_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_anon_user_target_account_name()) {
    anon_user_target_account_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_anon_user_target_account_name(),
      GetArena());
  }
  sha_sentryfile_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_sha_sentryfile()) {
    sha_sentryfile_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_sha_sentryfile(),
      GetArena());
  }
  auth_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_auth_code()) {
    auth_code_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_auth_code(),
      GetArena());
  }
  otp_identifier_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_otp_identifier()) {
    otp_identifier_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_otp_identifier(),
      GetArena());
  }
  sony_psn_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_sony_psn_ticket()) {
    sony_psn_ticket_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_sony_psn_ticket(),
      GetArena());
  }
  sony_psn_service_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_sony_psn_service_id()) {
    sony_psn_service_id_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_sony_psn_service_id(),
      GetArena());
  }
  sony_psn_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_sony_psn_name()) {
    sony_psn_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_sony_psn_name(),
      GetArena());
  }
  machine_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_machine_name()) {
    machine_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_machine_name(),
      GetArena());
  }
  machine_name_userchosen_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_machine_name_userchosen()) {
    machine_name_userchosen_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_machine_name_userchosen(),
      GetArena());
  }
  country_override_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_country_override()) {
    country_override_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_country_override(),
      GetArena());
  }
  two_factor_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_two_factor_code()) {
    two_factor_code_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_two_factor_code(),
      GetArena());
  }
  web_logon_nonce_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_web_logon_nonce()) {
    web_logon_nonce_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_web_logon_nonce(),
      GetArena());
  }
  access_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_access_token()) {
    access_token_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_access_token(),
      GetArena());
  }
  if (from._internal_has_obfuscated_private_ip()) {
    obfuscated_private_ip_ = new ::CMsgIPAddress(*from.obfuscated_private_ip_);
  } else {
    obfuscated_private_ip_ = nullptr;
  }
  if (from._internal_has_public_ip()) {
    public_ip_ = new ::CMsgIPAddress(*from.public_ip_);
  } else {
    public_ip_ = nullptr;
  }
  if (from._internal_has_embedded_client_secret()) {
    embedded_client_secret_ = new ::CMsgClientSecret(*from.embedded_client_secret_);
  } else {
    embedded_client_secret_ = nullptr;
  }
  ::memcpy(&protocol_version_, &from.protocol_version_,
    static_cast<size_t>(reinterpret_cast<char*>(&gaming_device_type_) -
    reinterpret_cast<char*>(&protocol_version_)) + sizeof(gaming_device_type_));

}

void CMsgClientLogon::SharedCtor() {
client_language_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
wine_version_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
machine_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
steam2_auth_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
email_address_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
account_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
password_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
game_server_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
login_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
anon_user_target_account_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
sha_sentryfile_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
auth_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
otp_identifier_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
sony_psn_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
sony_psn_service_id_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
sony_psn_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
machine_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
machine_name_userchosen_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
country_override_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
two_factor_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
web_logon_nonce_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
access_token_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&obfuscated_private_ip_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&gaming_device_type_) -
    reinterpret_cast<char*>(&obfuscated_private_ip_)) + sizeof(gaming_device_type_));
}

CMsgClientLogon::~CMsgClientLogon() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientLogon::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  client_language_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  wine_version_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  machine_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  steam2_auth_ticket_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  email_address_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  account_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  password_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  game_server_token_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  login_key_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  anon_user_target_account_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  sha_sentryfile_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  auth_code_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  otp_identifier_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  sony_psn_ticket_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  sony_psn_service_id_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  sony_psn_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  machine_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  machine_name_userchosen_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  country_override_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  two_factor_code_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  web_logon_nonce_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  access_token_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete obfuscated_private_ip_;
  if (this != internal_default_instance()) delete public_ip_;
  if (this != internal_default_instance()) delete embedded_client_secret_;
}

void CMsgClientLogon::ArenaDtor(void* object) {
  CMsgClientLogon* _this = reinterpret_cast< CMsgClientLogon* >(object);
  (void)_this;
}
void CMsgClientLogon::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientLogon::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientLogon::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      client_language_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      wine_version_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      machine_id_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      steam2_auth_ticket_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000010u) {
      email_address_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000020u) {
      account_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000040u) {
      password_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000080u) {
      game_server_token_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      login_key_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000200u) {
      anon_user_target_account_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000400u) {
      sha_sentryfile_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000800u) {
      auth_code_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00001000u) {
      otp_identifier_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00002000u) {
      sony_psn_ticket_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00004000u) {
      sony_psn_service_id_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00008000u) {
      sony_psn_name_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x00ff0000u) {
    if (cached_has_bits & 0x00010000u) {
      machine_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00020000u) {
      machine_name_userchosen_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00040000u) {
      country_override_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00080000u) {
      two_factor_code_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00100000u) {
      web_logon_nonce_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00200000u) {
      access_token_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00400000u) {
      GOOGLE_DCHECK(obfuscated_private_ip_ != nullptr);
      obfuscated_private_ip_->Clear();
    }
    if (cached_has_bits & 0x00800000u) {
      GOOGLE_DCHECK(public_ip_ != nullptr);
      public_ip_->Clear();
    }
  }
  if (cached_has_bits & 0x01000000u) {
    GOOGLE_DCHECK(embedded_client_secret_ != nullptr);
    embedded_client_secret_->Clear();
  }
  if (cached_has_bits & 0xfe000000u) {
    ::memset(&protocol_version_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&deprecated_10_) -
        reinterpret_cast<char*>(&protocol_version_)) + sizeof(deprecated_10_));
  }
  cached_has_bits = _has_bits_[1];
  if (cached_has_bits & 0x000000ffu) {
    ::memset(&deprecated_public_ip_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&should_remember_password_) -
        reinterpret_cast<char*>(&deprecated_public_ip_)) + sizeof(should_remember_password_));
  }
  if (cached_has_bits & 0x0000ff00u) {
    ::memset(&was_converted_deprecated_msg_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&game_server_app_id_) -
        reinterpret_cast<char*>(&was_converted_deprecated_msg_)) + sizeof(game_server_app_id_));
  }
  if (cached_has_bits & 0x00ff0000u) {
    ::memset(&client_instance_id_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&is_steam_deck_deprecated_) -
        reinterpret_cast<char*>(&client_instance_id_)) + sizeof(is_steam_deck_deprecated_));
  }
  if (cached_has_bits & 0x03000000u) {
    ::memset(&is_tesla_deprecated_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&gaming_device_type_) -
        reinterpret_cast<char*>(&is_tesla_deprecated_)) + sizeof(gaming_device_type_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientLogon::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_protocol_version(&_has_bits_);
          protocol_version_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_deprecated_obfustucated_private_ip(&_has_bits_);
          deprecated_obfustucated_private_ip_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_cell_id(&_has_bits_);
          cell_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_last_session_id(&_has_bits_);
          last_session_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_client_package_version(&_has_bits_);
          client_package_version_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 50)) {
          auto str = _internal_mutable_client_language();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_client_os_type(&_has_bits_);
          client_os_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_should_remember_password(&_has_bits_);
          should_remember_password_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          auto str = _internal_mutable_wine_version();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_deprecated_10(&_has_bits_);
          deprecated_10_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 90)) {
          ptr = ctx->ParseMessage(_internal_mutable_obfuscated_private_ip(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 160)) {
          _Internal::set_has_deprecated_public_ip(&_has_bits_);
          deprecated_public_ip_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 21:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 168)) {
          _Internal::set_has_qos_level(&_has_bits_);
          qos_level_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 22:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 177)) {
          _Internal::set_has_client_supplied_steam_id(&_has_bits_);
          client_supplied_steam_id_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 23:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 186)) {
          ptr = ctx->ParseMessage(_internal_mutable_public_ip(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 30:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 242)) {
          auto str = _internal_mutable_machine_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 31:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 248)) {
          _Internal::set_has_launcher_type(&_has_bits_);
          launcher_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 32:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 0)) {
          _Internal::set_has_ui_mode(&_has_bits_);
          ui_mode_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 33:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_chat_mode(&_has_bits_);
          chat_mode_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 41:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          auto str = _internal_mutable_steam2_auth_ticket();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 42:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 82)) {
          auto str = _internal_mutable_email_address();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 43:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 93)) {
          _Internal::set_has_rtime32_account_creation(&_has_bits_);
          rtime32_account_creation_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint32>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint32);
        } else goto handle_unusual;
        continue;

      case 50:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 146)) {
          auto str = _internal_mutable_account_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 51:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 154)) {
          auto str = _internal_mutable_password();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 52:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 162)) {
          auto str = _internal_mutable_game_server_token();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 60:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 226)) {
          auto str = _internal_mutable_login_key();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 70:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_was_converted_deprecated_msg(&_has_bits_);
          was_converted_deprecated_msg_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 80:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 130)) {
          auto str = _internal_mutable_anon_user_target_account_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 81:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 137)) {
          _Internal::set_has_resolved_user_steam_id(&_has_bits_);
          resolved_user_steam_id_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 82:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          _Internal::set_has_eresult_sentryfile(&_has_bits_);
          eresult_sentryfile_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 83:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 154)) {
          auto str = _internal_mutable_sha_sentryfile();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 84:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 162)) {
          auto str = _internal_mutable_auth_code();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 85:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 168)) {
          _Internal::set_has_otp_type(&_has_bits_);
          otp_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 86:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 176)) {
          _Internal::set_has_otp_value(&_has_bits_);
          otp_value_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 87:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 186)) {
          auto str = _internal_mutable_otp_identifier();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 88:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 192)) {
          _Internal::set_has_steam2_ticket_request(&_has_bits_);
          steam2_ticket_request_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 90:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 210)) {
          auto str = _internal_mutable_sony_psn_ticket();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 91:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 218)) {
          auto str = _internal_mutable_sony_psn_service_id();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 92:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 224)) {
          _Internal::set_has_create_new_psn_linked_account_if_needed(&_has_bits_);
          create_new_psn_linked_account_if_needed_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 93:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 234)) {
          auto str = _internal_mutable_sony_psn_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 94:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 240)) {
          _Internal::set_has_game_server_app_id(&_has_bits_);
          game_server_app_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 95:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 248)) {
          _Internal::set_has_steamguard_dont_remember_computer(&_has_bits_);
          steamguard_dont_remember_computer_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 96:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 2)) {
          auto str = _internal_mutable_machine_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 97:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_machine_name_userchosen();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 98:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_country_override();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 99:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_is_steam_box_deprecated(&_has_bits_);
          is_steam_box_deprecated_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 100:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_client_instance_id(&_has_bits_);
          client_instance_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 101:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          auto str = _internal_mutable_two_factor_code();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 102:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_supports_rate_limit_response(&_has_bits_);
          supports_rate_limit_response_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 103:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 58)) {
          auto str = _internal_mutable_web_logon_nonce();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 104:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 64)) {
          _Internal::set_has_priority_reason(&_has_bits_);
          priority_reason_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 105:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          ptr = ctx->ParseMessage(_internal_mutable_embedded_client_secret(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 106:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_disable_partner_autogrants(&_has_bits_);
          disable_partner_autogrants_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 107:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 88)) {
          _Internal::set_has_is_steam_deck_deprecated(&_has_bits_);
          is_steam_deck_deprecated_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 108:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 98)) {
          auto str = _internal_mutable_access_token();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 109:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 104)) {
          _Internal::set_has_is_chrome_os(&_has_bits_);
          is_chrome_os_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 110:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 112)) {
          _Internal::set_has_is_tesla_deprecated(&_has_bits_);
          is_tesla_deprecated_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 111:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 120)) {
          _Internal::set_has_gaming_device_type(&_has_bits_);
          gaming_device_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientLogon::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x02000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_protocol_version(), target);
  }

  if (cached_has_bits & 0x04000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_deprecated_obfustucated_private_ip(), target);
  }

  if (cached_has_bits & 0x08000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_cell_id(), target);
  }

  if (cached_has_bits & 0x10000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(4, this->_internal_last_session_id(), target);
  }

  if (cached_has_bits & 0x20000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(5, this->_internal_client_package_version(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        6, this->_internal_client_language(), target);
  }

  if (cached_has_bits & 0x40000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(7, this->_internal_client_os_type(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(8, this->_internal_should_remember_password(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        9, this->_internal_wine_version(), target);
  }

  if (cached_has_bits & 0x80000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(10, this->_internal_deprecated_10(), target);
  }

  if (cached_has_bits & 0x00400000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        11, _Internal::obfuscated_private_ip(this), target, stream);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(20, this->_internal_deprecated_public_ip(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(21, this->_internal_qos_level(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(22, this->_internal_client_supplied_steam_id(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00800000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        23, _Internal::public_ip(this), target, stream);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteBytesMaybeAliased(
        30, this->_internal_machine_id(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(31, this->_internal_launcher_type(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(32, this->_internal_ui_mode(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(33, this->_internal_chat_mode(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteBytesMaybeAliased(
        41, this->_internal_steam2_auth_ticket(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->WriteStringMaybeAliased(
        42, this->_internal_email_address(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed32ToArray(43, this->_internal_rtime32_account_creation(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000020u) {
    target = stream->WriteStringMaybeAliased(
        50, this->_internal_account_name(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->WriteStringMaybeAliased(
        51, this->_internal_password(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->WriteStringMaybeAliased(
        52, this->_internal_game_server_token(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->WriteStringMaybeAliased(
        60, this->_internal_login_key(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(70, this->_internal_was_converted_deprecated_msg(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000200u) {
    target = stream->WriteStringMaybeAliased(
        80, this->_internal_anon_user_target_account_name(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(81, this->_internal_resolved_user_steam_id(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(82, this->_internal_eresult_sentryfile(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000400u) {
    target = stream->WriteBytesMaybeAliased(
        83, this->_internal_sha_sentryfile(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->WriteStringMaybeAliased(
        84, this->_internal_auth_code(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(85, this->_internal_otp_type(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(86, this->_internal_otp_value(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00001000u) {
    target = stream->WriteStringMaybeAliased(
        87, this->_internal_otp_identifier(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(88, this->_internal_steam2_ticket_request(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00002000u) {
    target = stream->WriteBytesMaybeAliased(
        90, this->_internal_sony_psn_ticket(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->WriteStringMaybeAliased(
        91, this->_internal_sony_psn_service_id(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(92, this->_internal_create_new_psn_linked_account_if_needed(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00008000u) {
    target = stream->WriteStringMaybeAliased(
        93, this->_internal_sony_psn_name(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(94, this->_internal_game_server_app_id(), target);
  }

  if (cached_has_bits & 0x00020000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(95, this->_internal_steamguard_dont_remember_computer(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00010000u) {
    target = stream->WriteStringMaybeAliased(
        96, this->_internal_machine_name(), target);
  }

  if (cached_has_bits & 0x00020000u) {
    target = stream->WriteStringMaybeAliased(
        97, this->_internal_machine_name_userchosen(), target);
  }

  if (cached_has_bits & 0x00040000u) {
    target = stream->WriteStringMaybeAliased(
        98, this->_internal_country_override(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00400000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(99, this->_internal_is_steam_box_deprecated(), target);
  }

  if (cached_has_bits & 0x00010000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(100, this->_internal_client_instance_id(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00080000u) {
    target = stream->WriteStringMaybeAliased(
        101, this->_internal_two_factor_code(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00040000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(102, this->_internal_supports_rate_limit_response(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00100000u) {
    target = stream->WriteStringMaybeAliased(
        103, this->_internal_web_logon_nonce(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00200000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(104, this->_internal_priority_reason(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x01000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        105, _Internal::embedded_client_secret(this), target, stream);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00080000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(106, this->_internal_disable_partner_autogrants(), target);
  }

  if (cached_has_bits & 0x00800000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(107, this->_internal_is_steam_deck_deprecated(), target);
  }

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00200000u) {
    target = stream->WriteStringMaybeAliased(
        108, this->_internal_access_token(), target);
  }

  cached_has_bits = _has_bits_[1];

  if (cached_has_bits & 0x00100000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(109, this->_internal_is_chrome_os(), target);
  }

  if (cached_has_bits & 0x01000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(110, this->_internal_is_tesla_deprecated(), target);
  }

  if (cached_has_bits & 0x02000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(111, this->_internal_gaming_device_type(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientLogon::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_client_language());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_wine_version());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_machine_id());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_steam2_auth_ticket());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_email_address());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_account_name());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_password());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_game_server_token());
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_login_key());
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_anon_user_target_account_name());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_sha_sentryfile());
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_auth_code());
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_otp_identifier());
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_sony_psn_ticket());
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_sony_psn_service_id());
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_sony_psn_name());
    }

  }
  if (cached_has_bits & 0x00ff0000u) {

    if (cached_has_bits & 0x00010000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_machine_name());
    }

    if (cached_has_bits & 0x00020000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_machine_name_userchosen());
    }

    if (cached_has_bits & 0x00040000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_country_override());
    }

    if (cached_has_bits & 0x00080000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_two_factor_code());
    }

    if (cached_has_bits & 0x00100000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_web_logon_nonce());
    }

    if (cached_has_bits & 0x00200000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_access_token());
    }

    if (cached_has_bits & 0x00400000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *obfuscated_private_ip_);
    }

    if (cached_has_bits & 0x00800000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *public_ip_);
    }

  }
  if (cached_has_bits & 0xff000000u) {

    if (cached_has_bits & 0x01000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *embedded_client_secret_);
    }

    if (cached_has_bits & 0x02000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_protocol_version());
    }

    if (cached_has_bits & 0x04000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_deprecated_obfustucated_private_ip());
    }

    if (cached_has_bits & 0x08000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_cell_id());
    }

    if (cached_has_bits & 0x10000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_last_session_id());
    }

    if (cached_has_bits & 0x20000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_client_package_version());
    }

    if (cached_has_bits & 0x40000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_client_os_type());
    }

    if (cached_has_bits & 0x80000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_deprecated_10());
    }

  }
  cached_has_bits = _has_bits_[1];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_deprecated_public_ip());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 2 + 8;
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_qos_level());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_launcher_type());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_ui_mode());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_chat_mode());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 2 + 4;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 1 + 1;
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 2 + 8;
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_eresult_sentryfile());
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_otp_type());
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_otp_value());
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_game_server_app_id());
    }

  }
  if (cached_has_bits & 0x00ff0000u) {

    if (cached_has_bits & 0x00010000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_client_instance_id());
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
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00200000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_priority_reason());
    }

    if (cached_has_bits & 0x00400000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00800000u) {
      total_size += 2 + 1;
    }

  }
  if (cached_has_bits & 0x03000000u) {

    if (cached_has_bits & 0x01000000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x02000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_gaming_device_type());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientLogon::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientLogon*>(
      &from));
}

void CMsgClientLogon::MergeFrom(const CMsgClientLogon& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_client_language(from._internal_client_language());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_wine_version(from._internal_wine_version());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_machine_id(from._internal_machine_id());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_steam2_auth_ticket(from._internal_steam2_auth_ticket());
    }
    if (cached_has_bits & 0x00000010u) {
      _internal_set_email_address(from._internal_email_address());
    }
    if (cached_has_bits & 0x00000020u) {
      _internal_set_account_name(from._internal_account_name());
    }
    if (cached_has_bits & 0x00000040u) {
      _internal_set_password(from._internal_password());
    }
    if (cached_has_bits & 0x00000080u) {
      _internal_set_game_server_token(from._internal_game_server_token());
    }
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      _internal_set_login_key(from._internal_login_key());
    }
    if (cached_has_bits & 0x00000200u) {
      _internal_set_anon_user_target_account_name(from._internal_anon_user_target_account_name());
    }
    if (cached_has_bits & 0x00000400u) {
      _internal_set_sha_sentryfile(from._internal_sha_sentryfile());
    }
    if (cached_has_bits & 0x00000800u) {
      _internal_set_auth_code(from._internal_auth_code());
    }
    if (cached_has_bits & 0x00001000u) {
      _internal_set_otp_identifier(from._internal_otp_identifier());
    }
    if (cached_has_bits & 0x00002000u) {
      _internal_set_sony_psn_ticket(from._internal_sony_psn_ticket());
    }
    if (cached_has_bits & 0x00004000u) {
      _internal_set_sony_psn_service_id(from._internal_sony_psn_service_id());
    }
    if (cached_has_bits & 0x00008000u) {
      _internal_set_sony_psn_name(from._internal_sony_psn_name());
    }
  }
  if (cached_has_bits & 0x00ff0000u) {
    if (cached_has_bits & 0x00010000u) {
      _internal_set_machine_name(from._internal_machine_name());
    }
    if (cached_has_bits & 0x00020000u) {
      _internal_set_machine_name_userchosen(from._internal_machine_name_userchosen());
    }
    if (cached_has_bits & 0x00040000u) {
      _internal_set_country_override(from._internal_country_override());
    }
    if (cached_has_bits & 0x00080000u) {
      _internal_set_two_factor_code(from._internal_two_factor_code());
    }
    if (cached_has_bits & 0x00100000u) {
      _internal_set_web_logon_nonce(from._internal_web_logon_nonce());
    }
    if (cached_has_bits & 0x00200000u) {
      _internal_set_access_token(from._internal_access_token());
    }
    if (cached_has_bits & 0x00400000u) {
      _internal_mutable_obfuscated_private_ip()->::CMsgIPAddress::MergeFrom(from._internal_obfuscated_private_ip());
    }
    if (cached_has_bits & 0x00800000u) {
      _internal_mutable_public_ip()->::CMsgIPAddress::MergeFrom(from._internal_public_ip());
    }
  }
  if (cached_has_bits & 0xff000000u) {
    if (cached_has_bits & 0x01000000u) {
      _internal_mutable_embedded_client_secret()->::CMsgClientSecret::MergeFrom(from._internal_embedded_client_secret());
    }
    if (cached_has_bits & 0x02000000u) {
      protocol_version_ = from.protocol_version_;
    }
    if (cached_has_bits & 0x04000000u) {
      deprecated_obfustucated_private_ip_ = from.deprecated_obfustucated_private_ip_;
    }
    if (cached_has_bits & 0x08000000u) {
      cell_id_ = from.cell_id_;
    }
    if (cached_has_bits & 0x10000000u) {
      last_session_id_ = from.last_session_id_;
    }
    if (cached_has_bits & 0x20000000u) {
      client_package_version_ = from.client_package_version_;
    }
    if (cached_has_bits & 0x40000000u) {
      client_os_type_ = from.client_os_type_;
    }
    if (cached_has_bits & 0x80000000u) {
      deprecated_10_ = from.deprecated_10_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  cached_has_bits = from._has_bits_[1];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      deprecated_public_ip_ = from.deprecated_public_ip_;
    }
    if (cached_has_bits & 0x00000002u) {
      client_supplied_steam_id_ = from.client_supplied_steam_id_;
    }
    if (cached_has_bits & 0x00000004u) {
      qos_level_ = from.qos_level_;
    }
    if (cached_has_bits & 0x00000008u) {
      launcher_type_ = from.launcher_type_;
    }
    if (cached_has_bits & 0x00000010u) {
      ui_mode_ = from.ui_mode_;
    }
    if (cached_has_bits & 0x00000020u) {
      chat_mode_ = from.chat_mode_;
    }
    if (cached_has_bits & 0x00000040u) {
      rtime32_account_creation_ = from.rtime32_account_creation_;
    }
    if (cached_has_bits & 0x00000080u) {
      should_remember_password_ = from.should_remember_password_;
    }
    _has_bits_[1] |= cached_has_bits;
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      was_converted_deprecated_msg_ = from.was_converted_deprecated_msg_;
    }
    if (cached_has_bits & 0x00000200u) {
      steam2_ticket_request_ = from.steam2_ticket_request_;
    }
    if (cached_has_bits & 0x00000400u) {
      create_new_psn_linked_account_if_needed_ = from.create_new_psn_linked_account_if_needed_;
    }
    if (cached_has_bits & 0x00000800u) {
      resolved_user_steam_id_ = from.resolved_user_steam_id_;
    }
    if (cached_has_bits & 0x00001000u) {
      eresult_sentryfile_ = from.eresult_sentryfile_;
    }
    if (cached_has_bits & 0x00002000u) {
      otp_type_ = from.otp_type_;
    }
    if (cached_has_bits & 0x00004000u) {
      otp_value_ = from.otp_value_;
    }
    if (cached_has_bits & 0x00008000u) {
      game_server_app_id_ = from.game_server_app_id_;
    }
    _has_bits_[1] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00ff0000u) {
    if (cached_has_bits & 0x00010000u) {
      client_instance_id_ = from.client_instance_id_;
    }
    if (cached_has_bits & 0x00020000u) {
      steamguard_dont_remember_computer_ = from.steamguard_dont_remember_computer_;
    }
    if (cached_has_bits & 0x00040000u) {
      supports_rate_limit_response_ = from.supports_rate_limit_response_;
    }
    if (cached_has_bits & 0x00080000u) {
      disable_partner_autogrants_ = from.disable_partner_autogrants_;
    }
    if (cached_has_bits & 0x00100000u) {
      is_chrome_os_ = from.is_chrome_os_;
    }
    if (cached_has_bits & 0x00200000u) {
      priority_reason_ = from.priority_reason_;
    }
    if (cached_has_bits & 0x00400000u) {
      is_steam_box_deprecated_ = from.is_steam_box_deprecated_;
    }
    if (cached_has_bits & 0x00800000u) {
      is_steam_deck_deprecated_ = from.is_steam_deck_deprecated_;
    }
    _has_bits_[1] |= cached_has_bits;
  }
  if (cached_has_bits & 0x03000000u) {
    if (cached_has_bits & 0x01000000u) {
      is_tesla_deprecated_ = from.is_tesla_deprecated_;
    }
    if (cached_has_bits & 0x02000000u) {
      gaming_device_type_ = from.gaming_device_type_;
    }
    _has_bits_[1] |= cached_has_bits;
  }
}

void CMsgClientLogon::CopyFrom(const CMsgClientLogon& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientLogon::IsInitialized() const {
  return true;
}

void CMsgClientLogon::InternalSwap(CMsgClientLogon* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(_has_bits_[1], other->_has_bits_[1]);
  client_language_.Swap(&other->client_language_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  wine_version_.Swap(&other->wine_version_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  machine_id_.Swap(&other->machine_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  steam2_auth_ticket_.Swap(&other->steam2_auth_ticket_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  email_address_.Swap(&other->email_address_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  account_name_.Swap(&other->account_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  password_.Swap(&other->password_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  game_server_token_.Swap(&other->game_server_token_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  login_key_.Swap(&other->login_key_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  anon_user_target_account_name_.Swap(&other->anon_user_target_account_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  sha_sentryfile_.Swap(&other->sha_sentryfile_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  auth_code_.Swap(&other->auth_code_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  otp_identifier_.Swap(&other->otp_identifier_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  sony_psn_ticket_.Swap(&other->sony_psn_ticket_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  sony_psn_service_id_.Swap(&other->sony_psn_service_id_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  sony_psn_name_.Swap(&other->sony_psn_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  machine_name_.Swap(&other->machine_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  machine_name_userchosen_.Swap(&other->machine_name_userchosen_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  country_override_.Swap(&other->country_override_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  two_factor_code_.Swap(&other->two_factor_code_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  web_logon_nonce_.Swap(&other->web_logon_nonce_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  access_token_.Swap(&other->access_token_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgClientLogon, gaming_device_type_)
      + sizeof(CMsgClientLogon::gaming_device_type_)
      - PROTOBUF_FIELD_OFFSET(CMsgClientLogon, obfuscated_private_ip_)>(
          reinterpret_cast<char*>(&obfuscated_private_ip_),
          reinterpret_cast<char*>(&other->obfuscated_private_ip_));
}

std::string CMsgClientLogon::GetTypeName() const {
  return "CMsgClientLogon";
}

class CMsgClientLogonResponse::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientLogonResponse>()._has_bits_);
  static void set_has_eresult(HasBits* has_bits) {
    (*has_bits)[0] |= 67108864u;
  }
  static void set_has_legacy_out_of_game_heartbeat_seconds(HasBits* has_bits) {
    (*has_bits)[0] |= 512u;
  }
  static void set_has_heartbeat_seconds(HasBits* has_bits) {
    (*has_bits)[0] |= 1024u;
  }
  static void set_has_deprecated_public_ip(HasBits* has_bits) {
    (*has_bits)[0] |= 2048u;
  }
  static void set_has_rtime32_server_time(HasBits* has_bits) {
    (*has_bits)[0] |= 4096u;
  }
  static void set_has_account_flags(HasBits* has_bits) {
    (*has_bits)[0] |= 8192u;
  }
  static void set_has_cell_id(HasBits* has_bits) {
    (*has_bits)[0] |= 16384u;
  }
  static void set_has_email_domain(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_steam2_ticket(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_eresult_extended(HasBits* has_bits) {
    (*has_bits)[0] |= 32768u;
  }
  static void set_has_cell_id_ping_threshold(HasBits* has_bits) {
    (*has_bits)[0] |= 65536u;
  }
  static void set_has_deprecated_use_pics(HasBits* has_bits) {
    (*has_bits)[0] |= 262144u;
  }
  static void set_has_vanity_url(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static const ::CMsgIPAddress& public_ip(const CMsgClientLogonResponse* msg);
  static void set_has_public_ip(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
  static void set_has_user_country(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_client_supplied_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 131072u;
  }
  static void set_has_ip_country_code(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_parental_settings(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_parental_setting_signature(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_count_loginfailures_to_migrate(HasBits* has_bits) {
    (*has_bits)[0] |= 1048576u;
  }
  static void set_has_count_disconnects_to_migrate(HasBits* has_bits) {
    (*has_bits)[0] |= 2097152u;
  }
  static void set_has_ogs_data_report_time_window(HasBits* has_bits) {
    (*has_bits)[0] |= 4194304u;
  }
  static void set_has_client_instance_id(HasBits* has_bits) {
    (*has_bits)[0] |= 8388608u;
  }
  static void set_has_force_client_update_check(HasBits* has_bits) {
    (*has_bits)[0] |= 524288u;
  }
  static void set_has_agreement_session_url(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_token_id(HasBits* has_bits) {
    (*has_bits)[0] |= 16777216u;
  }
  static void set_has_family_group_id(HasBits* has_bits) {
    (*has_bits)[0] |= 33554432u;
  }
};

const ::CMsgIPAddress&
CMsgClientLogonResponse::_Internal::public_ip(const CMsgClientLogonResponse* msg) {
  return *msg->public_ip_;
}
void CMsgClientLogonResponse::clear_public_ip() {
  if (public_ip_ != nullptr) public_ip_->Clear();
  _has_bits_[0] &= ~0x00000100u;
}
CMsgClientLogonResponse::CMsgClientLogonResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientLogonResponse::CMsgClientLogonResponse(const CMsgClientLogonResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  email_domain_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_email_domain()) {
    email_domain_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_email_domain(),
      GetArena());
  }
  steam2_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_steam2_ticket()) {
    steam2_ticket_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_steam2_ticket(),
      GetArena());
  }
  vanity_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_vanity_url()) {
    vanity_url_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_vanity_url(),
      GetArena());
  }
  user_country_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_user_country()) {
    user_country_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_user_country(),
      GetArena());
  }
  ip_country_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_ip_country_code()) {
    ip_country_code_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_ip_country_code(),
      GetArena());
  }
  parental_settings_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_parental_settings()) {
    parental_settings_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_parental_settings(),
      GetArena());
  }
  parental_setting_signature_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_parental_setting_signature()) {
    parental_setting_signature_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_parental_setting_signature(),
      GetArena());
  }
  agreement_session_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_agreement_session_url()) {
    agreement_session_url_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_agreement_session_url(),
      GetArena());
  }
  if (from._internal_has_public_ip()) {
    public_ip_ = new ::CMsgIPAddress(*from.public_ip_);
  } else {
    public_ip_ = nullptr;
  }
  ::memcpy(&legacy_out_of_game_heartbeat_seconds_, &from.legacy_out_of_game_heartbeat_seconds_,
    static_cast<size_t>(reinterpret_cast<char*>(&eresult_) -
    reinterpret_cast<char*>(&legacy_out_of_game_heartbeat_seconds_)) + sizeof(eresult_));

}

void CMsgClientLogonResponse::SharedCtor() {
email_domain_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
steam2_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
vanity_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
user_country_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
ip_country_code_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
parental_settings_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
parental_setting_signature_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
agreement_session_url_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&public_ip_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&family_group_id_) -
    reinterpret_cast<char*>(&public_ip_)) + sizeof(family_group_id_));
eresult_ = 2;
}

CMsgClientLogonResponse::~CMsgClientLogonResponse() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientLogonResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  email_domain_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  steam2_ticket_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  vanity_url_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  user_country_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ip_country_code_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  parental_settings_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  parental_setting_signature_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  agreement_session_url_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete public_ip_;
}

void CMsgClientLogonResponse::ArenaDtor(void* object) {
  CMsgClientLogonResponse* _this = reinterpret_cast< CMsgClientLogonResponse* >(object);
  (void)_this;
}
void CMsgClientLogonResponse::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientLogonResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientLogonResponse::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      email_domain_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      steam2_ticket_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      vanity_url_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000008u) {
      user_country_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000010u) {
      ip_country_code_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000020u) {
      parental_settings_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000040u) {
      parental_setting_signature_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000080u) {
      agreement_session_url_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x00000100u) {
    GOOGLE_DCHECK(public_ip_ != nullptr);
    public_ip_->Clear();
  }
  if (cached_has_bits & 0x0000fe00u) {
    ::memset(&legacy_out_of_game_heartbeat_seconds_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&eresult_extended_) -
        reinterpret_cast<char*>(&legacy_out_of_game_heartbeat_seconds_)) + sizeof(eresult_extended_));
  }
  if (cached_has_bits & 0x00ff0000u) {
    ::memset(&cell_id_ping_threshold_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&client_instance_id_) -
        reinterpret_cast<char*>(&cell_id_ping_threshold_)) + sizeof(client_instance_id_));
  }
  if (cached_has_bits & 0x07000000u) {
    ::memset(&token_id_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&family_group_id_) -
        reinterpret_cast<char*>(&token_id_)) + sizeof(family_group_id_));
    eresult_ = 2;
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientLogonResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_eresult(&has_bits);
          eresult_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_legacy_out_of_game_heartbeat_seconds(&has_bits);
          legacy_out_of_game_heartbeat_seconds_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_heartbeat_seconds(&has_bits);
          heartbeat_seconds_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_deprecated_public_ip(&has_bits);
          deprecated_public_ip_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 45)) {
          _Internal::set_has_rtime32_server_time(&has_bits);
          rtime32_server_time_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint32>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint32);
        } else goto handle_unusual;
        continue;

      case 6:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 48)) {
          _Internal::set_has_account_flags(&has_bits);
          account_flags_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_cell_id(&has_bits);
          cell_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 8:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 66)) {
          auto str = _internal_mutable_email_domain();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 9:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 74)) {
          auto str = _internal_mutable_steam2_ticket();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 10:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 80)) {
          _Internal::set_has_eresult_extended(&has_bits);
          eresult_extended_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 12:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 96)) {
          _Internal::set_has_cell_id_ping_threshold(&has_bits);
          cell_id_ping_threshold_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 13:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 104)) {
          _Internal::set_has_deprecated_use_pics(&has_bits);
          deprecated_use_pics_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 14:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 114)) {
          auto str = _internal_mutable_vanity_url();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 122)) {
          ptr = ctx->ParseMessage(_internal_mutable_public_ip(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 130)) {
          auto str = _internal_mutable_user_country();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 20:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 161)) {
          _Internal::set_has_client_supplied_steamid(&has_bits);
          client_supplied_steamid_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
          ptr += sizeof(::PROTOBUF_NAMESPACE_ID::uint64);
        } else goto handle_unusual;
        continue;

      case 21:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 170)) {
          auto str = _internal_mutable_ip_country_code();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 22:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 178)) {
          auto str = _internal_mutable_parental_settings();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 23:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 186)) {
          auto str = _internal_mutable_parental_setting_signature();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 24:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 192)) {
          _Internal::set_has_count_loginfailures_to_migrate(&has_bits);
          count_loginfailures_to_migrate_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 25:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 200)) {
          _Internal::set_has_count_disconnects_to_migrate(&has_bits);
          count_disconnects_to_migrate_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 26:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 208)) {
          _Internal::set_has_ogs_data_report_time_window(&has_bits);
          ogs_data_report_time_window_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 27:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 216)) {
          _Internal::set_has_client_instance_id(&has_bits);
          client_instance_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 28:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 224)) {
          _Internal::set_has_force_client_update_check(&has_bits);
          force_client_update_check_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 29:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 234)) {
          auto str = _internal_mutable_agreement_session_url();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 30:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 240)) {
          _Internal::set_has_token_id(&has_bits);
          token_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 31:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 248)) {
          _Internal::set_has_family_group_id(&has_bits);
          family_group_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientLogonResponse::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x04000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_eresult(), target);
  }

  if (cached_has_bits & 0x00000200u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(2, this->_internal_legacy_out_of_game_heartbeat_seconds(), target);
  }

  if (cached_has_bits & 0x00000400u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(3, this->_internal_heartbeat_seconds(), target);
  }

  if (cached_has_bits & 0x00000800u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(4, this->_internal_deprecated_public_ip(), target);
  }

  if (cached_has_bits & 0x00001000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed32ToArray(5, this->_internal_rtime32_server_time(), target);
  }

  if (cached_has_bits & 0x00002000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(6, this->_internal_account_flags(), target);
  }

  if (cached_has_bits & 0x00004000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(7, this->_internal_cell_id(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        8, this->_internal_email_domain(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteBytesMaybeAliased(
        9, this->_internal_steam2_ticket(), target);
  }

  if (cached_has_bits & 0x00008000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(10, this->_internal_eresult_extended(), target);
  }

  if (cached_has_bits & 0x00010000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(12, this->_internal_cell_id_ping_threshold(), target);
  }

  if (cached_has_bits & 0x00040000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(13, this->_internal_deprecated_use_pics(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        14, this->_internal_vanity_url(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        15, _Internal::public_ip(this), target, stream);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->WriteStringMaybeAliased(
        16, this->_internal_user_country(), target);
  }

  if (cached_has_bits & 0x00020000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(20, this->_internal_client_supplied_steamid(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->WriteStringMaybeAliased(
        21, this->_internal_ip_country_code(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->WriteBytesMaybeAliased(
        22, this->_internal_parental_settings(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->WriteBytesMaybeAliased(
        23, this->_internal_parental_setting_signature(), target);
  }

  if (cached_has_bits & 0x00100000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(24, this->_internal_count_loginfailures_to_migrate(), target);
  }

  if (cached_has_bits & 0x00200000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(25, this->_internal_count_disconnects_to_migrate(), target);
  }

  if (cached_has_bits & 0x00400000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(26, this->_internal_ogs_data_report_time_window(), target);
  }

  if (cached_has_bits & 0x00800000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(27, this->_internal_client_instance_id(), target);
  }

  if (cached_has_bits & 0x00080000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(28, this->_internal_force_client_update_check(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->WriteStringMaybeAliased(
        29, this->_internal_agreement_session_url(), target);
  }

  if (cached_has_bits & 0x01000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(30, this->_internal_token_id(), target);
  }

  if (cached_has_bits & 0x02000000u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(31, this->_internal_family_group_id(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientLogonResponse::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_email_domain());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_steam2_ticket());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_vanity_url());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_user_country());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_ip_country_code());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_parental_settings());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_parental_setting_signature());
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_agreement_session_url());
    }

  }
  if (cached_has_bits & 0x0000ff00u) {

    if (cached_has_bits & 0x00000100u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
          *public_ip_);
    }

    if (cached_has_bits & 0x00000200u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_legacy_out_of_game_heartbeat_seconds());
    }

    if (cached_has_bits & 0x00000400u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_heartbeat_seconds());
    }

    if (cached_has_bits & 0x00000800u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_deprecated_public_ip());
    }

    if (cached_has_bits & 0x00001000u) {
      total_size += 1 + 4;
    }

    if (cached_has_bits & 0x00002000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_account_flags());
    }

    if (cached_has_bits & 0x00004000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_cell_id());
    }

    if (cached_has_bits & 0x00008000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_eresult_extended());
    }

  }
  if (cached_has_bits & 0x00ff0000u) {

    if (cached_has_bits & 0x00010000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_cell_id_ping_threshold());
    }

    if (cached_has_bits & 0x00020000u) {
      total_size += 2 + 8;
    }

    if (cached_has_bits & 0x00040000u) {
      total_size += 1 + 1;
    }

    if (cached_has_bits & 0x00080000u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00100000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_count_loginfailures_to_migrate());
    }

    if (cached_has_bits & 0x00200000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_count_disconnects_to_migrate());
    }

    if (cached_has_bits & 0x00400000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_ogs_data_report_time_window());
    }

    if (cached_has_bits & 0x00800000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_client_instance_id());
    }

  }
  if (cached_has_bits & 0x07000000u) {

    if (cached_has_bits & 0x01000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_token_id());
    }

    if (cached_has_bits & 0x02000000u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
          this->_internal_family_group_id());
    }

    if (cached_has_bits & 0x04000000u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_eresult());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientLogonResponse::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientLogonResponse*>(
      &from));
}

void CMsgClientLogonResponse::MergeFrom(const CMsgClientLogonResponse& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_email_domain(from._internal_email_domain());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_steam2_ticket(from._internal_steam2_ticket());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_vanity_url(from._internal_vanity_url());
    }
    if (cached_has_bits & 0x00000008u) {
      _internal_set_user_country(from._internal_user_country());
    }
    if (cached_has_bits & 0x00000010u) {
      _internal_set_ip_country_code(from._internal_ip_country_code());
    }
    if (cached_has_bits & 0x00000020u) {
      _internal_set_parental_settings(from._internal_parental_settings());
    }
    if (cached_has_bits & 0x00000040u) {
      _internal_set_parental_setting_signature(from._internal_parental_setting_signature());
    }
    if (cached_has_bits & 0x00000080u) {
      _internal_set_agreement_session_url(from._internal_agreement_session_url());
    }
  }
  if (cached_has_bits & 0x0000ff00u) {
    if (cached_has_bits & 0x00000100u) {
      _internal_mutable_public_ip()->::CMsgIPAddress::MergeFrom(from._internal_public_ip());
    }
    if (cached_has_bits & 0x00000200u) {
      legacy_out_of_game_heartbeat_seconds_ = from.legacy_out_of_game_heartbeat_seconds_;
    }
    if (cached_has_bits & 0x00000400u) {
      heartbeat_seconds_ = from.heartbeat_seconds_;
    }
    if (cached_has_bits & 0x00000800u) {
      deprecated_public_ip_ = from.deprecated_public_ip_;
    }
    if (cached_has_bits & 0x00001000u) {
      rtime32_server_time_ = from.rtime32_server_time_;
    }
    if (cached_has_bits & 0x00002000u) {
      account_flags_ = from.account_flags_;
    }
    if (cached_has_bits & 0x00004000u) {
      cell_id_ = from.cell_id_;
    }
    if (cached_has_bits & 0x00008000u) {
      eresult_extended_ = from.eresult_extended_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00ff0000u) {
    if (cached_has_bits & 0x00010000u) {
      cell_id_ping_threshold_ = from.cell_id_ping_threshold_;
    }
    if (cached_has_bits & 0x00020000u) {
      client_supplied_steamid_ = from.client_supplied_steamid_;
    }
    if (cached_has_bits & 0x00040000u) {
      deprecated_use_pics_ = from.deprecated_use_pics_;
    }
    if (cached_has_bits & 0x00080000u) {
      force_client_update_check_ = from.force_client_update_check_;
    }
    if (cached_has_bits & 0x00100000u) {
      count_loginfailures_to_migrate_ = from.count_loginfailures_to_migrate_;
    }
    if (cached_has_bits & 0x00200000u) {
      count_disconnects_to_migrate_ = from.count_disconnects_to_migrate_;
    }
    if (cached_has_bits & 0x00400000u) {
      ogs_data_report_time_window_ = from.ogs_data_report_time_window_;
    }
    if (cached_has_bits & 0x00800000u) {
      client_instance_id_ = from.client_instance_id_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x07000000u) {
    if (cached_has_bits & 0x01000000u) {
      token_id_ = from.token_id_;
    }
    if (cached_has_bits & 0x02000000u) {
      family_group_id_ = from.family_group_id_;
    }
    if (cached_has_bits & 0x04000000u) {
      eresult_ = from.eresult_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgClientLogonResponse::CopyFrom(const CMsgClientLogonResponse& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientLogonResponse::IsInitialized() const {
  return true;
}

void CMsgClientLogonResponse::InternalSwap(CMsgClientLogonResponse* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  email_domain_.Swap(&other->email_domain_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  steam2_ticket_.Swap(&other->steam2_ticket_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  vanity_url_.Swap(&other->vanity_url_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  user_country_.Swap(&other->user_country_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ip_country_code_.Swap(&other->ip_country_code_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  parental_settings_.Swap(&other->parental_settings_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  parental_setting_signature_.Swap(&other->parental_setting_signature_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  agreement_session_url_.Swap(&other->agreement_session_url_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgClientLogonResponse, family_group_id_)
      + sizeof(CMsgClientLogonResponse::family_group_id_)
      - PROTOBUF_FIELD_OFFSET(CMsgClientLogonResponse, public_ip_)>(
          reinterpret_cast<char*>(&public_ip_),
          reinterpret_cast<char*>(&other->public_ip_));
  swap(eresult_, other->eresult_);
}

std::string CMsgClientLogonResponse::GetTypeName() const {
  return "CMsgClientLogonResponse";
}

class CMsgClientRequestWebAPIAuthenticateUserNonce::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientRequestWebAPIAuthenticateUserNonce>()._has_bits_);
  static void set_has_token_type(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientRequestWebAPIAuthenticateUserNonce::CMsgClientRequestWebAPIAuthenticateUserNonce(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientRequestWebAPIAuthenticateUserNonce::CMsgClientRequestWebAPIAuthenticateUserNonce(const CMsgClientRequestWebAPIAuthenticateUserNonce& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  token_type_ = from.token_type_;

}

void CMsgClientRequestWebAPIAuthenticateUserNonce::SharedCtor() {
token_type_ = -1;
}

CMsgClientRequestWebAPIAuthenticateUserNonce::~CMsgClientRequestWebAPIAuthenticateUserNonce() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::ArenaDtor(void* object) {
  CMsgClientRequestWebAPIAuthenticateUserNonce* _this = reinterpret_cast< CMsgClientRequestWebAPIAuthenticateUserNonce* >(object);
  (void)_this;
}
void CMsgClientRequestWebAPIAuthenticateUserNonce::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientRequestWebAPIAuthenticateUserNonce::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  token_type_ = -1;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientRequestWebAPIAuthenticateUserNonce::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_token_type(&has_bits);
          token_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientRequestWebAPIAuthenticateUserNonce::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_token_type(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientRequestWebAPIAuthenticateUserNonce::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_token_type());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientRequestWebAPIAuthenticateUserNonce*>(
      &from));
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::MergeFrom(const CMsgClientRequestWebAPIAuthenticateUserNonce& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_token_type()) {
    _internal_set_token_type(from._internal_token_type());
  }
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::CopyFrom(const CMsgClientRequestWebAPIAuthenticateUserNonce& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientRequestWebAPIAuthenticateUserNonce::IsInitialized() const {
  return true;
}

void CMsgClientRequestWebAPIAuthenticateUserNonce::InternalSwap(CMsgClientRequestWebAPIAuthenticateUserNonce* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(token_type_, other->token_type_);
}

std::string CMsgClientRequestWebAPIAuthenticateUserNonce::GetTypeName() const {
  return "CMsgClientRequestWebAPIAuthenticateUserNonce";
}

class CMsgClientRequestWebAPIAuthenticateUserNonceResponse::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientRequestWebAPIAuthenticateUserNonceResponse>()._has_bits_);
  static void set_has_eresult(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_webapi_authenticate_user_nonce(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_token_type(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

CMsgClientRequestWebAPIAuthenticateUserNonceResponse::CMsgClientRequestWebAPIAuthenticateUserNonceResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientRequestWebAPIAuthenticateUserNonceResponse::CMsgClientRequestWebAPIAuthenticateUserNonceResponse(const CMsgClientRequestWebAPIAuthenticateUserNonceResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  webapi_authenticate_user_nonce_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_webapi_authenticate_user_nonce()) {
    webapi_authenticate_user_nonce_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_webapi_authenticate_user_nonce(),
      GetArena());
  }
  ::memcpy(&eresult_, &from.eresult_,
    static_cast<size_t>(reinterpret_cast<char*>(&token_type_) -
    reinterpret_cast<char*>(&eresult_)) + sizeof(token_type_));

}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::SharedCtor() {
webapi_authenticate_user_nonce_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
eresult_ = 2;
token_type_ = -1;
}

CMsgClientRequestWebAPIAuthenticateUserNonceResponse::~CMsgClientRequestWebAPIAuthenticateUserNonceResponse() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  webapi_authenticate_user_nonce_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::ArenaDtor(void* object) {
  CMsgClientRequestWebAPIAuthenticateUserNonceResponse* _this = reinterpret_cast< CMsgClientRequestWebAPIAuthenticateUserNonceResponse* >(object);
  (void)_this;
}
void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      webapi_authenticate_user_nonce_.ClearNonDefaultToEmpty();
    }
    eresult_ = 2;
    token_type_ = -1;
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientRequestWebAPIAuthenticateUserNonceResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_eresult(&has_bits);
          eresult_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_token_type(&has_bits);
          token_type_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 11:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 90)) {
          auto str = _internal_mutable_webapi_authenticate_user_nonce();
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientRequestWebAPIAuthenticateUserNonceResponse::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_eresult(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(3, this->_internal_token_type(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        11, this->_internal_webapi_authenticate_user_nonce(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientRequestWebAPIAuthenticateUserNonceResponse::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_webapi_authenticate_user_nonce());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_eresult());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_token_type());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientRequestWebAPIAuthenticateUserNonceResponse*>(
      &from));
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::MergeFrom(const CMsgClientRequestWebAPIAuthenticateUserNonceResponse& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_webapi_authenticate_user_nonce(from._internal_webapi_authenticate_user_nonce());
    }
    if (cached_has_bits & 0x00000002u) {
      eresult_ = from.eresult_;
    }
    if (cached_has_bits & 0x00000004u) {
      token_type_ = from.token_type_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::CopyFrom(const CMsgClientRequestWebAPIAuthenticateUserNonceResponse& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientRequestWebAPIAuthenticateUserNonceResponse::IsInitialized() const {
  return true;
}

void CMsgClientRequestWebAPIAuthenticateUserNonceResponse::InternalSwap(CMsgClientRequestWebAPIAuthenticateUserNonceResponse* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  webapi_authenticate_user_nonce_.Swap(&other->webapi_authenticate_user_nonce_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(eresult_, other->eresult_);
  swap(token_type_, other->token_type_);
}

std::string CMsgClientRequestWebAPIAuthenticateUserNonceResponse::GetTypeName() const {
  return "CMsgClientRequestWebAPIAuthenticateUserNonceResponse";
}

class CMsgClientLogOff::_Internal {
 public:
};

CMsgClientLogOff::CMsgClientLogOff(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientLogOff::CMsgClientLogOff(const CMsgClientLogOff& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite() {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);

}

void CMsgClientLogOff::SharedCtor() {
}

CMsgClientLogOff::~CMsgClientLogOff() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientLogOff::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientLogOff::ArenaDtor(void* object) {
  CMsgClientLogOff* _this = reinterpret_cast< CMsgClientLogOff* >(object);
  (void)_this;
}
void CMsgClientLogOff::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientLogOff::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientLogOff::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientLogOff::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
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
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientLogOff::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientLogOff::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientLogOff::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientLogOff*>(
      &from));
}

void CMsgClientLogOff::MergeFrom(const CMsgClientLogOff& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

}

void CMsgClientLogOff::CopyFrom(const CMsgClientLogOff& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientLogOff::IsInitialized() const {
  return true;
}

void CMsgClientLogOff::InternalSwap(CMsgClientLogOff* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
}

std::string CMsgClientLogOff::GetTypeName() const {
  return "CMsgClientLogOff";
}

class CMsgClientLoggedOff::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientLoggedOff>()._has_bits_);
  static void set_has_eresult(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientLoggedOff::CMsgClientLoggedOff(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientLoggedOff::CMsgClientLoggedOff(const CMsgClientLoggedOff& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  eresult_ = from.eresult_;

}

void CMsgClientLoggedOff::SharedCtor() {
eresult_ = 2;
}

CMsgClientLoggedOff::~CMsgClientLoggedOff() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientLoggedOff::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientLoggedOff::ArenaDtor(void* object) {
  CMsgClientLoggedOff* _this = reinterpret_cast< CMsgClientLoggedOff* >(object);
  (void)_this;
}
void CMsgClientLoggedOff::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientLoggedOff::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientLoggedOff::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  eresult_ = 2;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientLoggedOff::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_eresult(&has_bits);
          eresult_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientLoggedOff::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_eresult(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientLoggedOff::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_eresult());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientLoggedOff::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientLoggedOff*>(
      &from));
}

void CMsgClientLoggedOff::MergeFrom(const CMsgClientLoggedOff& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_eresult()) {
    _internal_set_eresult(from._internal_eresult());
  }
}

void CMsgClientLoggedOff::CopyFrom(const CMsgClientLoggedOff& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientLoggedOff::IsInitialized() const {
  return true;
}

void CMsgClientLoggedOff::InternalSwap(CMsgClientLoggedOff* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(eresult_, other->eresult_);
}

std::string CMsgClientLoggedOff::GetTypeName() const {
  return "CMsgClientLoggedOff";
}

class CMsgClientNewLoginKey::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientNewLoginKey>()._has_bits_);
  static void set_has_unique_id(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_login_key(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientNewLoginKey::CMsgClientNewLoginKey(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientNewLoginKey::CMsgClientNewLoginKey(const CMsgClientNewLoginKey& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  login_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_login_key()) {
    login_key_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_login_key(),
      GetArena());
  }
  unique_id_ = from.unique_id_;

}

void CMsgClientNewLoginKey::SharedCtor() {
login_key_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
unique_id_ = 0u;
}

CMsgClientNewLoginKey::~CMsgClientNewLoginKey() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientNewLoginKey::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  login_key_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgClientNewLoginKey::ArenaDtor(void* object) {
  CMsgClientNewLoginKey* _this = reinterpret_cast< CMsgClientNewLoginKey* >(object);
  (void)_this;
}
void CMsgClientNewLoginKey::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientNewLoginKey::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientNewLoginKey::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    login_key_.ClearNonDefaultToEmpty();
  }
  unique_id_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientNewLoginKey::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_unique_id(&has_bits);
          unique_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_login_key();
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientNewLoginKey::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_unique_id(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_login_key(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientNewLoginKey::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_login_key());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_unique_id());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientNewLoginKey::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientNewLoginKey*>(
      &from));
}

void CMsgClientNewLoginKey::MergeFrom(const CMsgClientNewLoginKey& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_login_key(from._internal_login_key());
    }
    if (cached_has_bits & 0x00000002u) {
      unique_id_ = from.unique_id_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void CMsgClientNewLoginKey::CopyFrom(const CMsgClientNewLoginKey& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientNewLoginKey::IsInitialized() const {
  return true;
}

void CMsgClientNewLoginKey::InternalSwap(CMsgClientNewLoginKey* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  login_key_.Swap(&other->login_key_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  swap(unique_id_, other->unique_id_);
}

std::string CMsgClientNewLoginKey::GetTypeName() const {
  return "CMsgClientNewLoginKey";
}

class CMsgClientNewLoginKeyAccepted::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientNewLoginKeyAccepted>()._has_bits_);
  static void set_has_unique_id(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientNewLoginKeyAccepted::CMsgClientNewLoginKeyAccepted(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientNewLoginKeyAccepted::CMsgClientNewLoginKeyAccepted(const CMsgClientNewLoginKeyAccepted& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  unique_id_ = from.unique_id_;

}

void CMsgClientNewLoginKeyAccepted::SharedCtor() {
unique_id_ = 0u;
}

CMsgClientNewLoginKeyAccepted::~CMsgClientNewLoginKeyAccepted() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientNewLoginKeyAccepted::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientNewLoginKeyAccepted::ArenaDtor(void* object) {
  CMsgClientNewLoginKeyAccepted* _this = reinterpret_cast< CMsgClientNewLoginKeyAccepted* >(object);
  (void)_this;
}
void CMsgClientNewLoginKeyAccepted::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientNewLoginKeyAccepted::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientNewLoginKeyAccepted::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  unique_id_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientNewLoginKeyAccepted::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_unique_id(&has_bits);
          unique_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientNewLoginKeyAccepted::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_unique_id(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientNewLoginKeyAccepted::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_unique_id());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientNewLoginKeyAccepted::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientNewLoginKeyAccepted*>(
      &from));
}

void CMsgClientNewLoginKeyAccepted::MergeFrom(const CMsgClientNewLoginKeyAccepted& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_unique_id()) {
    _internal_set_unique_id(from._internal_unique_id());
  }
}

void CMsgClientNewLoginKeyAccepted::CopyFrom(const CMsgClientNewLoginKeyAccepted& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientNewLoginKeyAccepted::IsInitialized() const {
  return true;
}

void CMsgClientNewLoginKeyAccepted::InternalSwap(CMsgClientNewLoginKeyAccepted* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(unique_id_, other->unique_id_);
}

std::string CMsgClientNewLoginKeyAccepted::GetTypeName() const {
  return "CMsgClientNewLoginKeyAccepted";
}

class CMsgClientAccountInfo::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientAccountInfo>()._has_bits_);
  static void set_has_persona_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_ip_country(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_count_authed_computers(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_account_flags(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_steamguard_machine_name_user_chosen(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_is_phone_verified(HasBits* has_bits) {
    (*has_bits)[0] |= 64u;
  }
  static void set_has_two_factor_state(HasBits* has_bits) {
    (*has_bits)[0] |= 32u;
  }
  static void set_has_is_phone_identifying(HasBits* has_bits) {
    (*has_bits)[0] |= 128u;
  }
  static void set_has_is_phone_needing_reverify(HasBits* has_bits) {
    (*has_bits)[0] |= 256u;
  }
};

CMsgClientAccountInfo::CMsgClientAccountInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientAccountInfo::CMsgClientAccountInfo(const CMsgClientAccountInfo& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  persona_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_persona_name()) {
    persona_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_persona_name(),
      GetArena());
  }
  ip_country_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_ip_country()) {
    ip_country_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_ip_country(),
      GetArena());
  }
  steamguard_machine_name_user_chosen_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_steamguard_machine_name_user_chosen()) {
    steamguard_machine_name_user_chosen_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_steamguard_machine_name_user_chosen(),
      GetArena());
  }
  ::memcpy(&count_authed_computers_, &from.count_authed_computers_,
    static_cast<size_t>(reinterpret_cast<char*>(&is_phone_needing_reverify_) -
    reinterpret_cast<char*>(&count_authed_computers_)) + sizeof(is_phone_needing_reverify_));

}

void CMsgClientAccountInfo::SharedCtor() {
persona_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
ip_country_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
steamguard_machine_name_user_chosen_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&count_authed_computers_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&is_phone_needing_reverify_) -
    reinterpret_cast<char*>(&count_authed_computers_)) + sizeof(is_phone_needing_reverify_));
}

CMsgClientAccountInfo::~CMsgClientAccountInfo() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientAccountInfo::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  persona_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ip_country_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  steamguard_machine_name_user_chosen_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void CMsgClientAccountInfo::ArenaDtor(void* object) {
  CMsgClientAccountInfo* _this = reinterpret_cast< CMsgClientAccountInfo* >(object);
  (void)_this;
}
void CMsgClientAccountInfo::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientAccountInfo::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientAccountInfo::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      persona_name_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      ip_country_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000004u) {
      steamguard_machine_name_user_chosen_.ClearNonDefaultToEmpty();
    }
  }
  if (cached_has_bits & 0x000000f8u) {
    ::memset(&count_authed_computers_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&is_phone_identifying_) -
        reinterpret_cast<char*>(&count_authed_computers_)) + sizeof(is_phone_identifying_));
  }
  is_phone_needing_reverify_ = false;
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientAccountInfo::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_persona_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_ip_country();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          _Internal::set_has_count_authed_computers(&has_bits);
          count_authed_computers_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 7:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 56)) {
          _Internal::set_has_account_flags(&has_bits);
          account_flags_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 15:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 122)) {
          auto str = _internal_mutable_steamguard_machine_name_user_chosen();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 16:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 128)) {
          _Internal::set_has_is_phone_verified(&has_bits);
          is_phone_verified_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 17:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 136)) {
          _Internal::set_has_two_factor_state(&has_bits);
          two_factor_state_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 18:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 144)) {
          _Internal::set_has_is_phone_identifying(&has_bits);
          is_phone_identifying_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 19:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 152)) {
          _Internal::set_has_is_phone_needing_reverify(&has_bits);
          is_phone_needing_reverify_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientAccountInfo::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_persona_name(), target);
  }

  if (cached_has_bits & 0x00000002u) {
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_ip_country(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(5, this->_internal_count_authed_computers(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(7, this->_internal_account_flags(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->WriteStringMaybeAliased(
        15, this->_internal_steamguard_machine_name_user_chosen(), target);
  }

  if (cached_has_bits & 0x00000040u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(16, this->_internal_is_phone_verified(), target);
  }

  if (cached_has_bits & 0x00000020u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(17, this->_internal_two_factor_state(), target);
  }

  if (cached_has_bits & 0x00000080u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(18, this->_internal_is_phone_identifying(), target);
  }

  if (cached_has_bits & 0x00000100u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteBoolToArray(19, this->_internal_is_phone_needing_reverify(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientAccountInfo::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_persona_name());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_ip_country());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
          this->_internal_steamguard_machine_name_user_chosen());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_count_authed_computers());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_account_flags());
    }

    if (cached_has_bits & 0x00000020u) {
      total_size += 2 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_two_factor_state());
    }

    if (cached_has_bits & 0x00000040u) {
      total_size += 2 + 1;
    }

    if (cached_has_bits & 0x00000080u) {
      total_size += 2 + 1;
    }

  }

  if (cached_has_bits & 0x00000100u) {
    total_size += 2 + 1;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientAccountInfo::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientAccountInfo*>(
      &from));
}

void CMsgClientAccountInfo::MergeFrom(const CMsgClientAccountInfo& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x000000ffu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_persona_name(from._internal_persona_name());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_set_ip_country(from._internal_ip_country());
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_set_steamguard_machine_name_user_chosen(from._internal_steamguard_machine_name_user_chosen());
    }
    if (cached_has_bits & 0x00000008u) {
      count_authed_computers_ = from.count_authed_computers_;
    }
    if (cached_has_bits & 0x00000010u) {
      account_flags_ = from.account_flags_;
    }
    if (cached_has_bits & 0x00000020u) {
      two_factor_state_ = from.two_factor_state_;
    }
    if (cached_has_bits & 0x00000040u) {
      is_phone_verified_ = from.is_phone_verified_;
    }
    if (cached_has_bits & 0x00000080u) {
      is_phone_identifying_ = from.is_phone_identifying_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  if (cached_has_bits & 0x00000100u) {
    _internal_set_is_phone_needing_reverify(from._internal_is_phone_needing_reverify());
  }
}

void CMsgClientAccountInfo::CopyFrom(const CMsgClientAccountInfo& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientAccountInfo::IsInitialized() const {
  return true;
}

void CMsgClientAccountInfo::InternalSwap(CMsgClientAccountInfo* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  persona_name_.Swap(&other->persona_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ip_country_.Swap(&other->ip_country_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  steamguard_machine_name_user_chosen_.Swap(&other->steamguard_machine_name_user_chosen_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(CMsgClientAccountInfo, is_phone_needing_reverify_)
      + sizeof(CMsgClientAccountInfo::is_phone_needing_reverify_)
      - PROTOBUF_FIELD_OFFSET(CMsgClientAccountInfo, count_authed_computers_)>(
          reinterpret_cast<char*>(&count_authed_computers_),
          reinterpret_cast<char*>(&other->count_authed_computers_));
}

std::string CMsgClientAccountInfo::GetTypeName() const {
  return "CMsgClientAccountInfo";
}

class CMsgClientChallengeRequest::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientChallengeRequest>()._has_bits_);
  static void set_has_steamid(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientChallengeRequest::CMsgClientChallengeRequest(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientChallengeRequest::CMsgClientChallengeRequest(const CMsgClientChallengeRequest& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  steamid_ = from.steamid_;

}

void CMsgClientChallengeRequest::SharedCtor() {
steamid_ = PROTOBUF_ULONGLONG(0);
}

CMsgClientChallengeRequest::~CMsgClientChallengeRequest() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientChallengeRequest::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientChallengeRequest::ArenaDtor(void* object) {
  CMsgClientChallengeRequest* _this = reinterpret_cast< CMsgClientChallengeRequest* >(object);
  (void)_this;
}
void CMsgClientChallengeRequest::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientChallengeRequest::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientChallengeRequest::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  steamid_ = PROTOBUF_ULONGLONG(0);
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientChallengeRequest::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientChallengeRequest::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(1, this->_internal_steamid(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientChallengeRequest::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 + 8;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientChallengeRequest::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientChallengeRequest*>(
      &from));
}

void CMsgClientChallengeRequest::MergeFrom(const CMsgClientChallengeRequest& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_steamid()) {
    _internal_set_steamid(from._internal_steamid());
  }
}

void CMsgClientChallengeRequest::CopyFrom(const CMsgClientChallengeRequest& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientChallengeRequest::IsInitialized() const {
  return true;
}

void CMsgClientChallengeRequest::InternalSwap(CMsgClientChallengeRequest* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(steamid_, other->steamid_);
}

std::string CMsgClientChallengeRequest::GetTypeName() const {
  return "CMsgClientChallengeRequest";
}

class CMsgClientChallengeResponse::_Internal {
 public:
  using HasBits = decltype(std::declval<CMsgClientChallengeResponse>()._has_bits_);
  static void set_has_challenge(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

CMsgClientChallengeResponse::CMsgClientChallengeResponse(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
CMsgClientChallengeResponse::CMsgClientChallengeResponse(const CMsgClientChallengeResponse& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  challenge_ = from.challenge_;

}

void CMsgClientChallengeResponse::SharedCtor() {
challenge_ = PROTOBUF_ULONGLONG(0);
}

CMsgClientChallengeResponse::~CMsgClientChallengeResponse() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void CMsgClientChallengeResponse::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void CMsgClientChallengeResponse::ArenaDtor(void* object) {
  CMsgClientChallengeResponse* _this = reinterpret_cast< CMsgClientChallengeResponse* >(object);
  (void)_this;
}
void CMsgClientChallengeResponse::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void CMsgClientChallengeResponse::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void CMsgClientChallengeResponse::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  challenge_ = PROTOBUF_ULONGLONG(0);
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* CMsgClientChallengeResponse::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 9)) {
          _Internal::set_has_challenge(&has_bits);
          challenge_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<::PROTOBUF_NAMESPACE_ID::uint64>(ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* CMsgClientChallengeResponse::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFixed64ToArray(1, this->_internal_challenge(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t CMsgClientChallengeResponse::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 + 8;
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void CMsgClientChallengeResponse::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const CMsgClientChallengeResponse*>(
      &from));
}

void CMsgClientChallengeResponse::MergeFrom(const CMsgClientChallengeResponse& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_challenge()) {
    _internal_set_challenge(from._internal_challenge());
  }
}

void CMsgClientChallengeResponse::CopyFrom(const CMsgClientChallengeResponse& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CMsgClientChallengeResponse::IsInitialized() const {
  return true;
}

void CMsgClientChallengeResponse::InternalSwap(CMsgClientChallengeResponse* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(challenge_, other->challenge_);
}

std::string CMsgClientChallengeResponse::GetTypeName() const {
  return "CMsgClientChallengeResponse";
}

PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::CMsgClientHeartBeat* Arena::CreateMaybeMessage< ::CMsgClientHeartBeat >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientHeartBeat >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientServerTimestampRequest* Arena::CreateMaybeMessage< ::CMsgClientServerTimestampRequest >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientServerTimestampRequest >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientServerTimestampResponse* Arena::CreateMaybeMessage< ::CMsgClientServerTimestampResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientServerTimestampResponse >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientSecret* Arena::CreateMaybeMessage< ::CMsgClientSecret >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientSecret >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientHello* Arena::CreateMaybeMessage< ::CMsgClientHello >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientHello >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientLogon* Arena::CreateMaybeMessage< ::CMsgClientLogon >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientLogon >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientLogonResponse* Arena::CreateMaybeMessage< ::CMsgClientLogonResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientLogonResponse >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientRequestWebAPIAuthenticateUserNonce* Arena::CreateMaybeMessage< ::CMsgClientRequestWebAPIAuthenticateUserNonce >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientRequestWebAPIAuthenticateUserNonce >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientRequestWebAPIAuthenticateUserNonceResponse* Arena::CreateMaybeMessage< ::CMsgClientRequestWebAPIAuthenticateUserNonceResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientRequestWebAPIAuthenticateUserNonceResponse >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientLogOff* Arena::CreateMaybeMessage< ::CMsgClientLogOff >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientLogOff >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientLoggedOff* Arena::CreateMaybeMessage< ::CMsgClientLoggedOff >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientLoggedOff >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientNewLoginKey* Arena::CreateMaybeMessage< ::CMsgClientNewLoginKey >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientNewLoginKey >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientNewLoginKeyAccepted* Arena::CreateMaybeMessage< ::CMsgClientNewLoginKeyAccepted >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientNewLoginKeyAccepted >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientAccountInfo* Arena::CreateMaybeMessage< ::CMsgClientAccountInfo >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientAccountInfo >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientChallengeRequest* Arena::CreateMaybeMessage< ::CMsgClientChallengeRequest >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientChallengeRequest >(arena);
}
template<> PROTOBUF_NOINLINE ::CMsgClientChallengeResponse* Arena::CreateMaybeMessage< ::CMsgClientChallengeResponse >(Arena* arena) {
  return Arena::CreateMessageInternal< ::CMsgClientChallengeResponse >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

#include <google/protobuf/port_undef.inc>
