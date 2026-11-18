

#include "../src/usermsg.hpp"

#include <cstdio>
#include <cstring>
#include <string>

static int g_failures = 0;

#define CHECK(cond, msg)                                                     \
	do {                                                                     \
		if (!(cond)) { std::printf("FAIL: %s\n", msg); ++g_failures; }       \
		else         { std::printf("ok:   %s\n", msg); }                     \
	} while (0)

static bool contains(const std::string& hay, const std::string& needle)
{
	return hay.find(needle) != std::string::npos;
}

static const UserMsg kAll[] = {
	UserMsg::LoadSuccess,
	UserMsg::BirthdayGreeting,
	UserMsg::SteamVersionUnsupported,
	UserMsg::SteamVersionMismatch,
	UserMsg::InitializationFailed,
	UserMsg::ContentServersUnavailable,
	UserMsg::DownloadAuthUnavailable,
	UserMsg::DownloadTimedOut,
	UserMsg::ManifestNotReady,
	UserMsg::GamePreparationFailed,
	UserMsg::DrmRemovalFailed,
	UserMsg::LocalStorageError,
	UserMsg::RuntimeDependencyMissing,
	UserMsg::ConfigUnreadable,
	UserMsg::ConfigParseFailed,
	UserMsg::ConfigRepaired,
	UserMsg::ConfigWriteFailed,
};

int main()
{

	{
		CHECK(detectLang("pt_BR.UTF-8") == Lang::Portuguese, "pt_BR -> Portuguese");
		CHECK(detectLang("pt_PT")       == Lang::Portuguese, "pt_PT -> Portuguese");
		CHECK(detectLang("pt")          == Lang::Portuguese, "pt -> Portuguese");
		CHECK(detectLang("en_US.UTF-8") == Lang::English,    "en_US -> English");
		CHECK(detectLang("de_DE")       == Lang::English,    "de_DE -> English (fallback)");
		CHECK(detectLang("")            == Lang::English,    "empty -> English");
		CHECK(detectLang(nullptr)       == Lang::English,    "null -> English");

		CHECK(detectLang("pt_BR:en")    == Lang::Portuguese, "colon list, pt first -> Portuguese");
	}

	{
		for (UserMsg m : kAll)
		{
			const UiMessage en = messageFor(m, Lang::English);
			const UiMessage pt = messageFor(m, Lang::Portuguese);
			CHECK(en.body && en.body[0] != '\0', "English body present");
			CHECK(pt.body && pt.body[0] != '\0', "Portuguese body present");
			CHECK(!contains(en.body, "depot=") && !contains(en.body, "gid=") &&
			      !contains(en.body, "ManifestFetch"),
			      "English body carries no raw developer token");
			CHECK(!contains(pt.body, "depot=") && !contains(pt.body, "gid=") &&
			      !contains(pt.body, "ManifestFetch"),
			      "Portuguese body carries no raw developer token");
		}
	}

	{
		const UiMessage en = messageFor(UserMsg::ContentServersUnavailable, Lang::English);
		const UiMessage pt = messageFor(UserMsg::ContentServersUnavailable, Lang::Portuguese);
		CHECK(std::strcmp(en.body, pt.body) != 0,
		      "ContentServersUnavailable differs EN vs PT");
	}

	{
		const UiMessage en = messageFor(UserMsg::ManifestNotReady, Lang::English);
		const UiMessage pt = messageFor(UserMsg::ManifestNotReady, Lang::Portuguese);
		CHECK(contains(en.body, "manifest") && contains(en.body, "try"),
		      "ManifestNotReady explains that a retry is needed");
		CHECK(contains(pt.body, "manifest") && contains(pt.body, "tente"),
		      "ManifestNotReady has actionable Portuguese wording");
	}

	{
		CHECK(substituteDetail("server said {detail}.", "HTTP 503") ==
		      "server said HTTP 503.",
		      "{detail} replaced with the runtime value");
		CHECK(substituteDetail("no placeholder here", "HTTP 503") ==
		      "no placeholder here",
		      "template without sentinel is unchanged");
		CHECK(!contains(substituteDetail("code {detail}", ""), "{detail}"),
		      "empty detail leaves no dangling sentinel");
	}

	{
		const UiMessage en = messageFor(UserMsg::ContentServersUnavailable, Lang::English);
		CHECK(contains(en.body, "{detail}"),
		      "ContentServersUnavailable body has a {detail} slot for the HTTP code");
	}

	{
		const UiMessage en = messageFor(UserMsg::GamePreparationFailed, Lang::English);
		const UiMessage pt = messageFor(UserMsg::GamePreparationFailed, Lang::Portuguese);
		CHECK(contains(en.body, "{detail}") && contains(pt.body, "{detail}"),
		      "GamePreparationFailed identifies the AppID");
		CHECK(contains(en.body, "Re-add") && contains(pt.body, "Adicione"),
		      "GamePreparationFailed asks the user to re-add the game");
		CHECK(!contains(en.body, "restart") && !contains(pt.body, "Reinicie") &&
		      !contains(pt.body, "reinicie"),
		      "GamePreparationFailed does not recommend restarting Steam");
	}

	{
		const UiMessage en = messageFor(UserMsg::GameMetadataUnavailable, Lang::English);
		const UiMessage pt = messageFor(UserMsg::GameMetadataUnavailable, Lang::Portuguese);
		CHECK(contains(en.body, "{detail}") && contains(pt.body, "{detail}"),
		      "GameMetadataUnavailable identifies the AppID");
		CHECK(contains(en.body, "connection") && contains(pt.body, "conex"),
		      "GameMetadataUnavailable explains the connectivity problem");
		CHECK(contains(en.body, "restart Steam") && contains(pt.body, "reinicie a Steam"),
		      "GameMetadataUnavailable explains how to refresh after reconnecting");
		CHECK(!contains(en.body, "Re-add") && !contains(pt.body, "Adicione"),
		      "GameMetadataUnavailable never recommends re-adding the title");
	}

	{
		CHECK(messageFor(UserMsg::LoadSuccess, Lang::English).severity == Severity::Info,
		      "LoadSuccess is Info severity");
		CHECK(messageFor(UserMsg::ContentServersUnavailable, Lang::English).severity == Severity::Error,
		      "ContentServersUnavailable is Error severity");
	}

	{
		CHECK(suppressedSuffix(Lang::English, 0).empty(),
		      "EN: zero suppressed -> no suffix");
		CHECK(suppressedSuffix(Lang::Portuguese, 0).empty(),
		      "PT: zero suppressed -> no suffix");

		CHECK(suppressedSuffix(Lang::English, 1) == "(and 1 more suppressed error)",
		      "EN: 1 suppressed -> singular");
		CHECK(suppressedSuffix(Lang::English, 3) == "(and 3 more suppressed errors)",
		      "EN: 3 suppressed -> plural");

		CHECK(suppressedSuffix(Lang::Portuguese, 1) == "(e mais 1 erro suprimido)",
		      "PT: 1 suppressed -> singular");
		CHECK(suppressedSuffix(Lang::Portuguese, 3) == "(e mais 3 erros suprimidos)",
		      "PT: 3 suppressed -> plural");
	}

	if (g_failures == 0) { std::printf("\nALL PASS\n"); return 0; }
	std::printf("\n%d CHECK(S) FAILED\n", g_failures);
	return 1;
}
