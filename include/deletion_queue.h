#pragma once

#include <functional>
#include <queue>

// Queue to handle deleting all the objects in the correct order
// Implemented like a stack (First In Last Out)
// Not optimal for large systems as storing separate function pointers
// is less efficient for memory
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); //call functors
		}

		deletors.clear();
	}
};
