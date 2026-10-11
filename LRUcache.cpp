
#include <bits/stdc++.h>
using namespace std;

int cacheSize;

// Stores pages from most recently used to least recently used
list<int> cache;

// Maps each page to its position in the cache
unordered_map<int, list<int>::iterator> pageMap;

// Handles a page access using LRU
void accessPage(int page) {
    // Page is not present in the cache
    if (pageMap.find(page) == pageMap.end()) {
        cout << "Cache Miss for page " << page << ".\n";
        cout << "\tAdding page to cache...\n";

        // If the cache is full, remove the least recently used page
        if (cache.size() == cacheSize) {
            int removedPage = cache.back();

            cout << "\tCache is full.\n";
            cout << "\tRemoved LRU page: " << removedPage << "\n";

            pageMap.erase(removedPage);
            cache.pop_back();
        }

        // Add the new page to the front
        cache.push_front(page);
        pageMap[page] = cache.begin();

        cout << "\tPage " << page << " added to cache.\n";
    }
    else {
        // Move the accessed page to the front
        cache.erase(pageMap[page]);
        cache.push_front(page);
        pageMap[page] = cache.begin();

        cout << "Cache Hit for page " << page
             << ". Recently used page moved to front.\n";
    }

    cout << "\n";
}

int main() {
    cout << "Enter cache size: ";
    cin >> cacheSize;

    if (cacheSize <= 0) {
        cout << "Cache size must be greater than zero.\n";
        return 1;
    }

    accessPage(1);
    accessPage(2);
    accessPage(3);
    accessPage(1);
    accessPage(4);
    accessPage(5);
    accessPage(5);
    accessPage(1);
    accessPage(6);
    accessPage(4);

    return 0;
}
