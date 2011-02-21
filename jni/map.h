#ifndef _MAP_H
#define	_MAP_H

#include "fontdef.h"
#include "uthash.h"

template <class K, class V>
class map {
  private:
    struct _Entry {
      K key;
      V value;
      UT_hash_handle hh;
    };

  public:
    class iterator {
      public:
        friend class map;
        iterator (_Entry* head)
          : current(head) {}

        K key () const {
          return current->key;
        }

        V value () const {
          return current->value;
        }

        void operator ++ (int) { //postfix ++
          current = (_Entry*)current->hh.next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Entry* current;
    };

    class const_iterator {
      public:
        friend class map;
        const_iterator (_Entry* head)
          : current(head) {}

        const K& key () const {
          return current->key;
        }

        const V& value () const {
          return current->value;
        }

        void operator ++ (int) { //postfix ++
          current = (_Entry*)current->hh.next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Entry* current;
    };

    map () : entries(NULL), _size(0) {}

    void insert (const K& key, const V& val) {
      if (contains(key))
        return;
      _Entry* e = new _Entry();
      e->key = key;
      e->value = val;
      HASH_ADD_KEYPTR(hh, entries, &e->key, sizeof(K), e);
      _size++;
    }

    bool contains (const K& key) const {
      _Entry* p = NULL;
      HASH_FIND(hh, entries, &key, sizeof(K), p);
      return p != NULL;
    }


    //get the object associated with the given key. If there is no such association,
    //an association is created with the default value
    const V& get (const K& key) {
      _Entry* p = NULL;
      HASH_FIND(hh, entries, &key, sizeof(K), p);
      if (p == NULL) {
        insert(key, V());
        HASH_FIND(hh, entries, &key, sizeof(K), p);
      }
      return p->value;
    }

    void remove (const K& key) {
      _Entry* p = NULL;
      HASH_FIND(hh, entries, &key, sizeof(K), p);
      if (p != NULL) {
        HASH_DELETE(hh, entries, p);
        _size--;
      }
    }

    size_t size () const {
      return _size;
    }

    iterator begin () {
      return iterator(entries);
    }

    const_iterator begin () const {
      return const_iterator(entries);
    }
    
  private:
    _Entry* entries;
    size_t _size;
};

#endif	/* _MAP_H */

