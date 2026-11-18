

#include "encrypted_app_ticket.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
constexpr EncryptedAppTicket::EncryptedAppTicket(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : encrypted_ticket_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , ticket_version_no_(0u)
  , crc_encryptedticket_(0u)
  , cb_encrypteduserdata_(0u)
  , cb_encrypted_appownershipticket_(0u){}
struct EncryptedAppTicketDefaultTypeInternal {
  constexpr EncryptedAppTicketDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~EncryptedAppTicketDefaultTypeInternal() {}
  union {
    EncryptedAppTicket _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT EncryptedAppTicketDefaultTypeInternal _EncryptedAppTicket_default_instance_;

class EncryptedAppTicket::_Internal {
 public:
  using HasBits = decltype(std::declval<EncryptedAppTicket>()._has_bits_);
  static void set_has_ticket_version_no(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_crc_encryptedticket(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_cb_encrypteduserdata(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_cb_encrypted_appownershipticket(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static void set_has_encrypted_ticket(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

EncryptedAppTicket::EncryptedAppTicket(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);

}
EncryptedAppTicket::EncryptedAppTicket(const EncryptedAppTicket& from)
  : ::PROTOBUF_NAMESPACE_ID::MessageLite(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  encrypted_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_encrypted_ticket()) {
    encrypted_ticket_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_encrypted_ticket(),
      GetArena());
  }
  ::memcpy(&ticket_version_no_, &from.ticket_version_no_,
    static_cast<size_t>(reinterpret_cast<char*>(&cb_encrypted_appownershipticket_) -
    reinterpret_cast<char*>(&ticket_version_no_)) + sizeof(cb_encrypted_appownershipticket_));

}

void EncryptedAppTicket::SharedCtor() {
encrypted_ticket_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&ticket_version_no_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&cb_encrypted_appownershipticket_) -
    reinterpret_cast<char*>(&ticket_version_no_)) + sizeof(cb_encrypted_appownershipticket_));
}

EncryptedAppTicket::~EncryptedAppTicket() {

  SharedDtor();
  _internal_metadata_.Delete<std::string>();
}

void EncryptedAppTicket::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  encrypted_ticket_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void EncryptedAppTicket::ArenaDtor(void* object) {
  EncryptedAppTicket* _this = reinterpret_cast< EncryptedAppTicket* >(object);
  (void)_this;
}
void EncryptedAppTicket::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void EncryptedAppTicket::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void EncryptedAppTicket::Clear() {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    encrypted_ticket_.ClearNonDefaultToEmpty();
  }
  if (cached_has_bits & 0x0000001eu) {
    ::memset(&ticket_version_no_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&cb_encrypted_appownershipticket_) -
        reinterpret_cast<char*>(&ticket_version_no_)) + sizeof(cb_encrypted_appownershipticket_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<std::string>();
}

const char* EncryptedAppTicket::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {

      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_ticket_version_no(&has_bits);
          ticket_version_no_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_crc_encryptedticket(&has_bits);
          crc_encryptedticket_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_cb_encrypteduserdata(&has_bits);
          cb_encrypteduserdata_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_cb_encrypted_appownershipticket(&has_bits);
          cb_encrypted_appownershipticket_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;

      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          auto str = _internal_mutable_encrypted_ticket();
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

::PROTOBUF_NAMESPACE_ID::uint8* EncryptedAppTicket::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];

  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_ticket_version_no(), target);
  }

  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_crc_encryptedticket(), target);
  }

  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(3, this->_internal_cb_encrypteduserdata(), target);
  }

  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(4, this->_internal_cb_encrypted_appownershipticket(), target);
  }

  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        5, this->_internal_encrypted_ticket(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = stream->WriteRaw(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).data(),
        static_cast<int>(_internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size()), target);
  }

  return target;
}

size_t EncryptedAppTicket::ByteSizeLong() const {

  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;

  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {

    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_encrypted_ticket());
    }

    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_ticket_version_no());
    }

    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_crc_encryptedticket());
    }

    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_cb_encrypteduserdata());
    }

    if (cached_has_bits & 0x00000010u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
          this->_internal_cb_encrypted_appownershipticket());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    total_size += _internal_metadata_.unknown_fields<std::string>(::PROTOBUF_NAMESPACE_ID::internal::GetEmptyString).size();
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void EncryptedAppTicket::CheckTypeAndMergeFrom(
    const ::PROTOBUF_NAMESPACE_ID::MessageLite& from) {
  MergeFrom(*::PROTOBUF_NAMESPACE_ID::internal::DownCast<const EncryptedAppTicket*>(
      &from));
}

void EncryptedAppTicket::MergeFrom(const EncryptedAppTicket& from) {

  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<std::string>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_encrypted_ticket(from._internal_encrypted_ticket());
    }
    if (cached_has_bits & 0x00000002u) {
      ticket_version_no_ = from.ticket_version_no_;
    }
    if (cached_has_bits & 0x00000004u) {
      crc_encryptedticket_ = from.crc_encryptedticket_;
    }
    if (cached_has_bits & 0x00000008u) {
      cb_encrypteduserdata_ = from.cb_encrypteduserdata_;
    }
    if (cached_has_bits & 0x00000010u) {
      cb_encrypted_appownershipticket_ = from.cb_encrypted_appownershipticket_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void EncryptedAppTicket::CopyFrom(const EncryptedAppTicket& from) {

  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool EncryptedAppTicket::IsInitialized() const {
  return true;
}

void EncryptedAppTicket::InternalSwap(EncryptedAppTicket* other) {
  using std::swap;
  _internal_metadata_.Swap<std::string>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  encrypted_ticket_.Swap(&other->encrypted_ticket_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(EncryptedAppTicket, cb_encrypted_appownershipticket_)
      + sizeof(EncryptedAppTicket::cb_encrypted_appownershipticket_)
      - PROTOBUF_FIELD_OFFSET(EncryptedAppTicket, ticket_version_no_)>(
          reinterpret_cast<char*>(&ticket_version_no_),
          reinterpret_cast<char*>(&other->ticket_version_no_));
}

std::string EncryptedAppTicket::GetTypeName() const {
  return "EncryptedAppTicket";
}

PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::EncryptedAppTicket* Arena::CreateMaybeMessage< ::EncryptedAppTicket >(Arena* arena) {
  return Arena::CreateMessageInternal< ::EncryptedAppTicket >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

#include <google/protobuf/port_undef.inc>
