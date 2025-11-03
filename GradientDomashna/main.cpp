#include <bits/stdc++.h>

using namespace std;

// ---------------- Selection Sort ----------------
void selectionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[minIndex])
                minIndex = j;
        }
        swap(arr[i], arr[minIndex]);
    }
}

// ---------------- Insertion Sort ----------------
void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ---------------- Merge Sort ----------------
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// ---------------- Quick Sort ----------------
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// ---------------- Benchmark Helper ----------------
template <typename Func>
void benchmark(Func sortFunc, vector<int> arr, const string& name) {
    auto start = chrono::high_resolution_clock::now();
    sortFunc(arr);
    auto stop = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = stop - start;
    cout << name << " took " << duration.count() << " seconds\n";
}

int main() {
    const int N = 100000;
    vector<int> arr(N);

    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(1, 1000000);

    for (int i = 0; i < N; i++) {
        arr[i] = dist(rng);
    }

    cout << "Sorting " << N << " random numbers...\n";

    benchmark([&](vector<int> v){ selectionSort(v); }, arr, "Selection Sort");
    benchmark([&](vector<int> v){ insertionSort(v); }, arr, "Insertion Sort");
    benchmark([&](vector<int> v){ mergeSort(v, 0, v.size() - 1); }, arr, "Merge Sort");
    benchmark([&](vector<int> v){ quickSort(v, 0, v.size() - 1); }, arr, "Quick Sort");
    benchmark([&](vector<int> v){ sort(v.begin(), v.end()); }, arr, "std::sort");

    return 0;
}
