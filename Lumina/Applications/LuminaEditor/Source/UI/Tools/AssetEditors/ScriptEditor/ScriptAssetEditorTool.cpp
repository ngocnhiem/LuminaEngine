#include "ScriptAssetEditorTool.h"

#include "Assets/AssetTypes/Script/ScriptAsset.h"
#include "Core/Object/Cast.h"
#include "Tools/UI/ImGui/ImGuiFonts.h"


namespace Lumina
{
    static const char* ScriptEditorName        = "ScriptEditor";
    
    
    FScriptAssetEditorTool::FScriptAssetEditorTool(IEditorToolContext* Context, CObject* InAsset)
        : FAssetEditorTool(Context, InAsset->GetName().ToString(), InAsset)
    {
        TextEditor.SetLanguageDefinition(CreateLuaLanguageDef());
    	TextEditor.SetPalette(TextEditor::GetDarkPalette());
    }

    void FScriptAssetEditorTool::OnInitialize()
    {
        CreateToolWindow(ScriptEditorName, [this](const FUpdateContext& Cxt, bool bFocused)
        {
            DrawScriptEditorWindow();
        });

        CScriptAsset* ScriptAsset = Cast<CScriptAsset>(Asset.Get());

        TextEditor.SetText(ScriptAsset->GetScript().c_str());
    }

    void FScriptAssetEditorTool::OnDeinitialize(const FUpdateContext& UpdateContext)
    {
    }

    void FScriptAssetEditorTool::OnAssetLoadFinished()
    {
        FAssetEditorTool::OnAssetLoadFinished();
    }

    void FScriptAssetEditorTool::DrawToolMenu(const FUpdateContext& UpdateContext)
    {
        if (ImGui::BeginMenu(LE_ICON_FORMAT_SIZE "Text Size"))
        {
            ImGui::DragFloat("Size", &TextSize, 1, 8, 64);

            ImGui::EndMenu();
        }
    }

    void FScriptAssetEditorTool::InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const
    {
        ImGui::DockBuilderDockWindow(GetToolWindowName(ScriptEditorName).c_str(), InDockspaceID);
    }

    const TextEditor::LanguageDefinition& FScriptAssetEditorTool::CreateLuaLanguageDef()
    {
    	static bool inited = false;
		static TextEditor::LanguageDefinition langDef;
		if (!inited)
		{
			static const char* const keywords[] =
			{
				"and", "break", "do", "", "else", "elseif", "end", "false", "for", "function", "if", "in", "", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while"
			};
	
			for (auto& k : keywords)
			{
				langDef.mKeywords.insert(k);
			}

			static const char* const Builtins[] =
			{
				"assert", "collectgarbage", "dofile", "error", "getmetatable", "ipairs", "loadfile", "load", "loadstring",  "next",  "pairs",  "pcall",  "print",  "rawequal",  "rawlen",  "rawget",  "rawset",
				"select",  "setmetatable",  "tonumber",  "tostring",  "type",  "xpcall",  "_G",  "_VERSION","arshift", "band", "bnot", "bor", "bxor", "btest", "extract", "lrotate", "lshift", "replace",
				"rrotate", "rshift", "create", "resume", "running", "status", "wrap", "yield", "isyieldable", "debug","getuservalue", "gethook", "getinfo", "getlocal", "getregistry", "getmetatable",
				"getupvalue", "upvaluejoin", "upvalueid", "setuservalue", "sethook", "setlocal", "setmetatable", "setupvalue", "traceback", "close", "flush", "input", "lines", "open", "output", "popen",
				"read", "tmpfile", "type", "write", "close", "flush", "lines", "read", "seek", "setvbuf", "write", "__gc", "__tostring", "abs", "acos", "asin", "atan", "ceil", "cos", "deg", "exp", "tointeger",
				"floor", "fmod", "ult", "log", "max", "min", "modf", "rad", "random", "randomseed", "sin", "sqrt", "string", "tan", "type", "atan2", "cosh", "sinh", "tanh",
				"pow", "frexp", "ldexp", "log10", "pi", "huge", "maxinteger", "mininteger", "loadlib", "searchpath", "seeall", "preload", "cpath", "path", "searchers", "loaded", "module", "require", "clock",
				"date", "difftime", "execute", "exit", "getenv", "remove", "rename", "setlocale", "time", "tmpname", "byte", "char", "dump", "find", "format", "gmatch", "gsub", "len", "lower", "match", "rep",
				"reverse", "sub", "upper", "pack", "packsize", "unpack", "concat", "maxn", "insert", "pack", "unpack", "remove", "move", "sort", "offset", "codepoint", "char", "len", "codes", "charpattern",
				"coroutine", "table", "io", "os", "string", "utf8", "bit32", "math", "debug", "package"
			};
			
			for (auto& k : Builtins)
			{
				TextEditor::Identifier id;
				id.mDeclaration = "Built-in function";
				langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
			}

			sol::state& LuaState = Scripting::FScriptingContext::Get().GetState();
			sol::state_view View(LuaState);
			View.for_each([&](sol::object Key, sol::object Value)
			{
				if (!Key.is<std::string>())
				{
					return;
				}

				std::string KeyStr = Key.as<std::string>();
    
				TextEditor::Identifier id;
				id.mDeclaration = "Global Identifier";
				langDef.mIdentifiers.insert(std::make_pair(KeyStr, id));
			});

			
	
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", TextEditor::PaletteIndex::String));
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\\'[^\\\']*\\\'", TextEditor::PaletteIndex::String));
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", TextEditor::PaletteIndex::Number));
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", TextEditor::PaletteIndex::Number));
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", TextEditor::PaletteIndex::Number));
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", TextEditor::PaletteIndex::Identifier));
			langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", TextEditor::PaletteIndex::Punctuation));
	
			langDef.mCommentStart = "--[[";
			langDef.mCommentEnd = "]]";
			langDef.mSingleLineComment = "--";
	
			langDef.mCaseSensitive = true;
			langDef.mAutoIndentation = false;
	
			langDef.mName = "Lua";
	
			inited = true;
		}
		return langDef;
    }

    void FScriptAssetEditorTool::OnSave()
    {
        CScriptAsset* ScriptAsset = Cast<CScriptAsset>(Asset.Get());
        ScriptAsset->Script = TextEditor.GetText().c_str();
        
        FAssetEditorTool::OnSave();

        ScriptAsset->PostScriptReloaded.Broadcast();
    }

    void FScriptAssetEditorTool::DrawScriptEditorWindow()
    {
        ImGui::PushFontSize(TextSize);
        TextEditor.Render("Script Editor");
        ImGui::PopFontSize();
    }
}
