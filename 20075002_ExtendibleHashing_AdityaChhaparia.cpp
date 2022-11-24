#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef int a_int;
#define int long long
#pragma GCC optimize("O2")
#define CLR(s) memset(&s, 0, sizeof(s))
#define sz(x) ((long long)(x).size())
#define all(x) x.begin(), x.end()
#define rall(x) (x).rbegin(), (x).rend()
#define trav(a, x) for (auto &a : x)
#define rep(i, l, r) for (ll i = l; i < r; i++)
#define fr(i, l, r) for (int i = l; i < r; i++)
#define nrep(i, l, r) for (ll i = l; i >= r; i--)
#define ed << "\n"
#define sp << " "
#define ff first
#define ss second
#define mp make_pair
#define pb push_back
#define ppb pop_back
#define setbits(x) __builtin_popcountll(x)
#define zrobits(x) __builtin_ctzll(x)
#define leastsigbit(x) __builtin_ffs(x)

class Bucket
{
public:
  int size;
  int depth;
  vector<int> data;

  Bucket(int size, int depth)
  {
    this->size = size;
    this->depth = depth;
  }

  int insert(int val)
  {
    if (this->size == sz(this->data))
    {
      return 0;
    }
    this->data.pb(val);
    return 1;
  }

  int has(int val)
  {
    for (auto i : data)
    {
      if (i == val)
        return 1;
    }
    return 0;
  }

  int remove(int val)
  {
    int n = sz(this->data);

    if (n == 0)
      return 0;
    fr(i, 0, n)
    {
      if (this->data[i] == val)
      {
        fr(j, i, n - 1)
        {
          swap(this->data[j], this->data[j + 1]);
        }
        this->data.ppb();
        return 1;
      }
    }

    return 0;
  }
};

class Directory
{
public:
  int size;
  int depth;
  vector<Bucket *> buffer;

  Directory(int size, int depth)
  {
    this->size = size;
    this->depth = depth;

    int n = (1 << depth);
    this->buffer.resize(n);

    fr(i, 0, n)
    {
      buffer[i] = new Bucket(size, depth);
    }
  }
};

Directory grow(Directory prev, int idx)
{
  int newDepth = prev.depth + 1;
  int size = prev.size;

  Directory next(size, newDepth);

  fr(i, 0, sz(prev.buffer))
  {
    if (i == idx)
      continue;
    int firstIdx = i;
    int secondIdx = i + (1 << (newDepth - 1));

    Bucket *bucket = prev.buffer[i];

    next.buffer[firstIdx] = bucket;
    next.buffer[secondIdx] = bucket;
  }

  int firstIdx = idx;
  int secondIdx = idx + (1 << (newDepth - 1));

  next.buffer[firstIdx] = new Bucket(size, newDepth);
  next.buffer[secondIdx] = new Bucket(size, newDepth);

  Bucket *oldBucket = prev.buffer[idx];

  for (auto data : oldBucket->data)
  {
    int h2 = ((1 << (newDepth)) - 1);
    int indx = data & h2;
    next.buffer[indx]->insert(data);
  }
  // delete oldBucket;
  return next;
}

Directory split(Directory prev, int idx)
{
  int depth = prev.depth;
  int firstIdx = idx;
  int secondIdx = -1;

  int h = 1 << (depth - 1);
  int flag = idx & h;

  if (flag == 0)
  {
    secondIdx = idx + h;
  }
  // secondIdx = idx + h;

  Bucket *oldBucket = prev.buffer[idx];

  prev.buffer[firstIdx] =
      new Bucket(oldBucket->size, oldBucket->depth + 1);
  prev.buffer[secondIdx] =
      new Bucket(oldBucket->size, oldBucket->depth + 1);

  vector<int> oldData = oldBucket->data;

  for (auto data : oldData)
  {
    int h2 = ((1 << (oldBucket->depth + 1)) - 1);

    int indx = data & h2;
    prev.buffer[indx]->insert(data);
  }

  // delete oldBucket;
  return prev;
}

Directory merge(Directory prev, int indx1, int indx2)
{
  vector<int> data;
  Bucket *b1 = prev.buffer[indx1];
  Bucket *b2 = prev.buffer[indx2];

  for (auto i : b1->data)
  {
    data.pb(i);
  }

  for (auto i : b2->data)
  {
    data.pb(i);
  }

  prev.buffer[indx1] = new Bucket(b1->size, b1->depth - 1);
  prev.buffer[indx2] = prev.buffer[indx1];

  int depth = prev.depth;

  for (auto i : data)
  {
    int indx = i & ((1 << depth) - 1);
    prev.buffer[indx]->insert(i);
  }

  // delete b1;
  // delete b2;

  return prev;
}

Directory shrink(Directory prev, int indx1, int indx2)
{
  prev = merge(prev, indx1, indx2);
  Directory next(prev.size, prev.depth - 1);

  fr(i, 0, (1 << next.depth))
  {
    next.buffer[i] = prev.buffer[i];
  }

  return next;
}

class ExtendibleHashing
{
private:
  vector<bool> order;

public:
  int hash(int idx, int depth)
  {
    return idx & ((1 << (depth)) - 1);
  }

  int hash2(int idx, int depth)
  {
    return (idx + (1 << (depth - 1))) / (1 << depth);
  }

  Directory d = Directory(3, 1);

  void insert(int data)
  {
    int indx = this->hash(data, d.depth);
    int flag = this->d.buffer[indx]->insert(data);
    if (flag)
      return;

    if (d.buffer[indx]->depth == d.depth)
    {
      d = grow(d, indx);
      order.pb(1);
    }
    else
    {
      d = split(d, indx);
      order.pb(0);
    }
    insert(data);
  }

  void display()
  {
    int depth = d.depth;
    int size = 1 << depth;

    fr(i, 0, size)
    {
      auto data = d.buffer[i]->data;
      cout << i << " --> ";
      for (auto k : data)
      {
        cout << k << ' ';
      }
      cout << endl;
    }
  }

  bool remove(int data)
  {
    int idx = hash(data, d.depth);

    Bucket *bucket = d.buffer[idx];

    int flag = bucket->remove(data);
    if (!flag)
    {
      return false;
    }

    int idx2 = hash2(idx, d.depth);
    Bucket *bucket2 = d.buffer[idx2];

    if (bucket2 == bucket)
      return true;

    if (sz(bucket->data) + sz(bucket2->data) > bucket->size)
    {
      order.ppb();
      return true;
    }

    if (bucket->depth == d.depth && order.back() == 1)
    {
      this->d = shrink(this->d, idx, idx2);
    }
    else
    {
      this->d = merge(this->d, idx, idx2);
    }
    order.ppb();

    return true;
  }
};

a_int main()
{
  ExtendibleHashing hash;
  vector<int> arr = {1, 10, 5, 11, 32, 89, 62, 79, 84, 53, 91};

  for (auto i : arr)
  {
    hash.insert(i);
    // hash.display();
  }

  // hash.display();
  // cout << endl;
  hash.remove(91);
  // hash.display();
  // cout << endl;
  hash.remove(5);
  // hash.display();
  // cout << endl;
  hash.remove(11);
  hash.display();
  return 0;
}