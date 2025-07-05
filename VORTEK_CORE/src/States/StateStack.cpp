#include "Core/States/StateStack.h"
#include <Logger/Logger.h>

namespace VORTEK_CORE
{

void StateStack::Push( State& state )
{
	auto hasState = std::ranges::find_if( m_States, [ & ]( const auto& s ) { return s.sName == state.sName; } );

	if ( hasState == m_States.end() )
	{
		state.bAddState = true;
		m_pStateHolder = std::make_unique<State>( state );
		return;
	}

	VORTEK_ASSERT( false && "Trying to add a state that is already in the stack!" );
}

void StateStack::Pop()
{
	if ( m_States.empty() )
	{
		VORTEK_ERROR( "Trying to pop an empty state stack!" );
		return;
	}

	auto& top = m_States.back();
	top.bKillState = true;
}

void StateStack::ChangeState( State& state )
{
	if ( !m_States.empty() )
		Pop();

	Push( state );
}

void StateStack::RemoveState( const std::string& sState )
{
	auto stateItr = std::ranges::find_if( m_States, [ & ]( State& state ) { return state.sName == sState; } );
	if ( stateItr == m_States.end() )
	{
		VORTEK_ERROR( "Failed to remove state [{}] - Does not exist or is invalid.", sState );
		return;
	}

	stateItr->bKillState = true;
}

void StateStack::Update( const float dt )
{
	if ( m_pStateHolder && m_pStateHolder->bAddState )
	{
		m_States.push_back( *m_pStateHolder );

		if ( m_pStateHolder->on_enter.valid() )
		{
			try
			{
				auto result = m_pStateHolder->on_enter();
				if ( !result.valid() )
				{
					sol::error error = result;
					throw error;
				}
			}
			catch ( const sol::error& error )
			{
				VORTEK_ERROR( error.what() );
			}

			m_pStateHolder = nullptr;
		}
	}

	if ( m_States.empty() )
		return;

	auto& topState = m_States.back();
	if ( topState.on_update.valid() )
	{
		try
		{
			auto result = topState.on_update( dt );
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Failed to update top state: {}", error.what() );
			return;
		}
		catch ( const std::exception& ex )
		{
			VORTEK_ERROR( "Failed to update top state: {}", ex.what() );
			return;
		}
		catch ( ... )
		{
			VORTEK_ERROR( "Failed to update top state: Unknown Error" );
			return;
		}
	}

	if ( topState.handle_inputs.valid() )
	{
		try
		{
			auto result = topState.handle_inputs();
			if ( !result.valid() )
			{
				sol::error error = result;
				throw error;
			}
		}
		catch ( const sol::error& error )
		{
			VORTEK_ERROR( "Failed to handle top state's inputs: {}", error.what() );
			return;
		}
	}

	if ( topState.bKillState )
	{
		if ( topState.on_exit.valid() )
		{
			try
			{
				auto result = topState.on_exit();
				if ( !result.valid() )
				{
					sol::error error = result;
					throw error;
				}
			}
			catch ( const sol::error& error )
			{
				VORTEK_ERROR( "Failed to exit top state: {}", error.what() );
				return;
			}
		}

		m_States.pop_back();
	}
}

void StateStack::Render()
{
	for ( const auto& state : m_States )
	{
		if ( state.on_render.valid() )
		{
			try
			{
				auto result = state.on_render();
				if ( !result.valid() )
				{
					sol::error error = result;
					throw error;
				}
			}
			catch ( const sol::error& error )
			{
				VORTEK_ERROR( "Failed to render state [{}]: {}", state.sName, error.what() );
			}
		}
	}
}

State& StateStack::Top()
{
	VORTEK_ASSERT( !m_States.empty() && "Cannot get the top of an empty stack" );

	if ( m_States.empty() )
		throw std::runtime_error( "State stack is empty!" );

	return m_States.back();
}

void StateStack::CreateLuaStateStackBind( sol::state& lua )
{
	lua.new_usertype<StateStack>( "StateStack",
								  sol::call_constructor,
								  sol::constructors<StateStack()>(),
								  "changeState",
								  &StateStack::ChangeState,
								  "push",
								  &StateStack::Push,
								  "pop",
								  &StateStack::Pop,
								  "update",
								  &StateStack::Update,
								  "render",
								  &StateStack::Render,
								  "top",
								  &StateStack::Top );
}
} // namespace VORTEK_CORE
