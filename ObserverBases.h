/* Uses the Observer pattern
* Observer is the base class observer interface
* Subject is the base class of things to watch
* Subject will use notify to tell all of the observers in their list that something happened
* Every observer then decides what to do with the notifications
*/
#pragma once
#include <vector>

// only interface classes

enum class Event { shoot, move, win, lose };	// This is too project specific
									// Would rather this be abstract to be adaptable
namespace Wump {
	class Actor;

	class Observer {	// Base class for all observers
	public:
		Observer() {  }
		virtual void OnNotify(Actor*, Event) = 0;
		virtual ~Observer() {  }
	};

	class Subject {
	public:
		using ObserverList = std::vector<Observer*>;
		Subject() {  }
		virtual void RegisterObserver(Observer*) = 0;
		virtual void UnregisterObserver(Observer*) = 0;
		virtual ~Subject() {  }
	protected:
		virtual void Notify(Actor* actor, Event whatHappened) {
			for (auto it : observers) {
				it->OnNotify(actor, whatHappened);
			}
		}
		ObserverList observers;
	};
}
