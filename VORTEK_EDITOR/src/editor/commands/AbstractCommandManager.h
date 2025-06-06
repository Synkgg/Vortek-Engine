#pragma once

#include <variant>
#include <stack>

namespace VORTEK_EDITOR
{
	template <typename T>
	concept CommandType = requires(T t) {
		{
			t.undo()
		} -> std::same_as<void>;
		{
			t.redo()
		} -> std::same_as<void>;
	};

	template <CommandType... Cmds>
	using VarCommands = std::variant<Cmds...>;

	template <CommandType... Cmds>
	using CommandStack = std::stack<VarCommands<Cmds...>>;

	template <CommandType... Cmds>
	class AbstractCommandManager
	{
	public:
		void Undo();
		void Redo();
		void Clear();

		bool UndoEmpty() const noexcept { return m_UndoStack.empty(); }

		bool RedoEmpty() const noexcept { return m_RedoStack.empty(); }

		void Execute(VarCommands<Cmds...>& undoableVariant);

	private:
		void RunUndo(VarCommands<Cmds...>& undoVariant);

		template <CommandType TCmd>
		void RunUndo_Impl(TCmd& undoCmd);

		void RunRedo(VarCommands<Cmds...>& undoVariant);

		template <CommandType TCmd>
		void RunRedo_Impl(TCmd& undoCmd);

	private:
		CommandStack<Cmds...> m_UndoStack;
		CommandStack<Cmds...> m_RedoStack;
	};


} // namespace VORTEK_EDITOR

#include "AbstractCommandManager.inl"