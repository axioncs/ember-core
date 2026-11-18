#pragma once

#include <string>

enum class Lang { English, Portuguese };
enum class Severity { Info, Warning, Error };

enum class UserMsg
{

	LoadSuccess,
	BirthdayGreeting,

	SteamVersionUnsupported,
	SteamVersionMismatch,
	InitializationFailed,

	ContentServersUnavailable,
	DownloadAuthUnavailable,
	DownloadTimedOut,
	ManifestNotReady,
	GameMetadataUnavailable,
	GamePreparationFailed,

	GamePreparationFailedBatch,
	DrmRemovalFailed,
	LocalStorageError,
	RuntimeDependencyMissing,

	ConfigUnreadable,
	ConfigParseFailed,
	ConfigRepaired,
	ConfigWriteFailed,
};

struct UiMessage
{
	const char* body;
	Severity    severity;
};

inline Lang detectLang(const char* env)
{
	if (!env || env[0] == '\0') return Lang::English;

	if ((env[0] == 'p' || env[0] == 'P') && (env[1] == 't' || env[1] == 'T'))
	{

		const char c = env[2];
		if (c == '\0' || c == '_' || c == '.' || c == ':' || c == '-' || c == ' ')
		{
			return Lang::Portuguese;
		}
	}
	return Lang::English;
}

inline std::string substituteDetail(const std::string& tmpl, const std::string& detail)
{
	static const std::string kSentinel = "{detail}";
	const auto pos = tmpl.find(kSentinel);
	if (pos == std::string::npos) return tmpl;

	std::string out = tmpl;
	if (detail.empty())
	{
		size_t eraseFrom = pos;
		size_t eraseLen  = kSentinel.size();

		if (eraseFrom > 0 && out[eraseFrom - 1] == ' ')
		{
			eraseFrom -= 1;
			eraseLen  += 1;
		}
		out.erase(eraseFrom, eraseLen);
	}
	else
	{
		out.replace(pos, kSentinel.size(), detail);
	}
	return out;
}

inline std::string suppressedSuffix(Lang lang, int n)
{
	if (n <= 0) return {};
	const std::string count = std::to_string(n);
	if (lang == Lang::Portuguese)
	{
		return n == 1 ? "(e mais " + count + " erro suprimido)"
		              : "(e mais " + count + " erros suprimidos)";
	}
	return n == 1 ? "(and " + count + " more suppressed error)"
	              : "(and " + count + " more suppressed errors)";
}

inline UiMessage messageFor(UserMsg m, Lang lang)
{
	const bool pt = (lang == Lang::Portuguese);
	switch (m)
	{
		case UserMsg::LoadSuccess:
			return { pt ? "ember carregou com sucesso."
			            : "ember loaded successfully.",
			         Severity::Info };

		case UserMsg::BirthdayGreeting:
			return { pt ? "ember carregou com sucesso. (E feliz aniversário, ember!)"
			            : "ember loaded successfully. (And happy birthday, ember!)",
			         Severity::Info };

		case UserMsg::SteamVersionUnsupported:
			return { pt ? "O ember foi desativado por segurança porque esta "
			              "versão do Steam não é compatível."
			            : "ember was disabled for safety because this Steam "
			              "version isn't compatible.",
			         Severity::Error };

		case UserMsg::SteamVersionMismatch:
			return { pt ? "A Steam foi atualizada e ainda não validamos esta versão. "
			              "A maioria das coisas deve funcionar, mas se algo falhar, "
			              "procure uma atualização."
			            : "Steam was updated and we haven't validated this version yet. "
			              "Most things should still work; if something breaks, check "
			              "for an update.",
			         Severity::Warning };

		case UserMsg::InitializationFailed:
			return { pt ? "O ember não encontrou os padrões de código da "
			              "Steam, então ela está rodando sem modificações. Isso "
			              "costuma acontecer quando a Steam se atualiza. Saia do canal "
			              "beta da Steam e rode o comando de instalação novamente."
			            : "ember couldn't find Steam's code patterns, so Steam "
			              "is running without any modifications. This usually happens "
			              "after a Steam update. Leave the Steam beta channel and run "
			              "the install command again.",
			         Severity::Error };

		case UserMsg::ContentServersUnavailable:
			return { pt ? "Os servidores de conteúdo da Steam estão temporariamente "
			              "indisponíveis ({detail}). Aguarde alguns minutos e tente "
			              "baixar o jogo novamente."
			            : "Steam's content servers are temporarily unavailable "
			              "({detail}). Please wait a few minutes and try downloading "
			              "the game again.",
			         Severity::Error };

		case UserMsg::DownloadAuthUnavailable:
			return { pt ? "Não foi possível obter autorização de download junto à "
			              "Steam agora. Aguarde um momento e tente baixar o jogo "
			              "novamente."
			            : "Couldn't get download authorization from Steam right now. "
			              "Please wait a moment and try downloading the game again.",
			         Severity::Error };

		case UserMsg::DownloadTimedOut:
			return { pt ? "A Steam demorou demais para responder. Verifique sua "
			              "conexão e tente baixar o jogo novamente."
			            : "Steam took too long to respond. Check your connection and "
			              "try downloading the game again.",
			         Severity::Error };

		case UserMsg::ManifestNotReady:
			return { pt ? "Um ou mais manifests desta versão ainda não estão no "
			              "arquivo compartilhado. Aguarde um pouco e tente instalar "
			              "novamente."
			            : "One or more manifests for this version aren't in the shared "
			              "archive yet. Please wait a little and try installing again.",
			         Severity::Warning };

		case UserMsg::GameMetadataUnavailable:
			return { pt ? "Não foi possível atualizar os dados do AppID {detail} "
			              "sem conexão. Conecte-se à internet e reinicie a Steam."
			            : "Couldn't update AppID {detail} data without a network "
			              "connection. Reconnect and restart Steam.",
			         Severity::Warning };

		case UserMsg::GamePreparationFailed:
			return { pt ? "O AppID {detail} tem dados de instalação incompletos. "
			              "Adicione o jogo novamente antes de instalar."
			            : "AppID {detail} has incomplete install data. Re-add the game "
			              "before installing.",
			         Severity::Error };

		case UserMsg::GamePreparationFailedBatch:
			return { pt ? "{detail} jogos não têm conteúdo instalável e foram "
			              "ignorados. Os demais continuam funcionando normalmente."
			            : "{detail} games have no installable content and were "
			              "skipped. The rest continue to work normally.",
			         Severity::Warning };

		case UserMsg::DrmRemovalFailed:
			return { pt ? "Não foi possível remover o DRM de um jogo, então ele pode "
			              "não abrir. Tente iniciá-lo de novo; se persistir, "
			              "reinstale-o."
			            : "Couldn't remove the game's DRM, so it may not launch. Try "
			              "starting it again; if it keeps failing, reinstall it.",
			         Severity::Error };

		case UserMsg::LocalStorageError:
			return { pt ? "Falha ao gravar arquivos no disco. Verifique se há espaço "
			              "livre e permissões na pasta da Steam."
			            : "Failed to write files to disk. Check that you have free "
			              "space and permissions in your Steam folder.",
			         Severity::Error };

		case UserMsg::RuntimeDependencyMissing:
			return { pt ? "Faltam ferramentas necessárias ({detail}) para processar "
			              "manifests. Instale-as e reinicie a Steam."
			            : "Required tools ({detail}) are missing for manifest processing. "
			              "Install them and restart Steam.",
			         Severity::Error };

		case UserMsg::ConfigUnreadable:
			return { pt ? "Não foi possível ler o arquivo de configuração; usando os "
			              "padrões por enquanto."
			            : "Couldn't read the configuration file; using defaults for now.",
			         Severity::Warning };

		case UserMsg::ConfigParseFailed:
			return { pt ? "O arquivo de configuração tem um erro de formatação; usando "
			              "os padrões por enquanto."
			            : "The configuration file has a formatting error; using "
			              "defaults for now.",
			         Severity::Warning };

		case UserMsg::ConfigRepaired:
			return { pt ? "O arquivo de configuração tinha um problema de formatação; "
			              "corrigimos automaticamente e mantivemos seus jogos."
			            : "The configuration file had a formatting problem; we fixed it "
			              "automatically and kept your games.",
			         Severity::Warning };

		case UserMsg::ConfigWriteFailed:
			return { pt ? "Não foi possível salvar a configuração. Verifique as "
			              "permissões da sua pasta pessoal."
			            : "Couldn't save the configuration. Check the permissions on "
			              "your home folder.",
			         Severity::Error };
	}

	return { pt ? "Ocorreu um erro inesperado."
	            : "An unexpected error occurred.",
	         Severity::Error };
}
