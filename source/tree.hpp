#pragma once

#include <functional>
#include <utility>
#include <list>

#include <4u/la/vec.hpp>
#include <4u/util/op.hpp>

#ifdef __TREE_DEBUG
#include <iostream>
#include <4u/la/mat.hpp>
#include <graphics/graphics.h>
#include <media/log.h>
#endif

struct TreeKey
{
public:
	vec2 position;
	double size;
	TreeKey() {}
	TreeKey(const vec2 &p, double s) : position(p), size(s) {}
	bool isSuitableDown(const TreeKey &k) const
	{
		vec2 rel = k.position - position;
		return (size >= k.size) && (_max(rel.x(),rel.y()) <= size) && (_min(rel.x(),rel.y()) > -size);
	}
	bool isSuitable(const TreeKey &k) const
	{
		return (size < 2*k.size) && isSuitableDown(k);
	}
};

template <typename T>
class Tree
{
public:
	typedef TreeKey Key;
	
private:
	class Branch
	{
	public:
		Key key;
		Branch *parent = nullptr;
		Branch *offspring = nullptr;
		Branch *neighbours[8] = {nullptr};
		std::list<std::pair<Key,T>> elements;
		bool empty = true;
	};
	
	Branch root;
	int depth;
	
	static void __create_tree(Branch *root, int level)
	{
		if(level > 0)
		{
			root->offspring = new Branch[4];
			const Key &key = root->key;
			for(int i = 0; i < 4; ++i)
			{
				root->offspring[i].parent = root;
				root->offspring[i].key = 
				    Key(key.position + key.size*vec2(i%2 - 0.5, i/2 - 0.5), 0.5*key.size);
				__create_tree(&root->offspring[i],level - 1);
			}
		}
	}
	
	static void __find_neighbours(Branch *root, Branch *b)
	{
		for(int i = 0; i < 8; ++i)
		{
			Key k = b->key;
			int j = i + (i > 3);
			k.position += 2.0*vec2(j%3 - 1, j/3 - 1)*k.size;
			Branch *nb = __find_branch(root,k);
			if(nb->key.isSuitable(k))
			{
				b->neighbours[i] = nb;
			}
		}
		if(b->offspring)
		{
			for(int i = 0; i < 4; ++i)
			{
				__find_neighbours(root,&b->offspring[i]);
			}
		}
	}
	
	static void __destroy_tree(Branch *root)
	{
		if(root->offspring)
		{
			for(int i = 0; i < 4; ++i)
			{
				__destroy_tree(&root->offspring[i]);
			}
			delete[] root->offspring;
			root->offspring = nullptr;
		}
	}
	
	static int __get_index(vec2 rel)
	{
		return int(rel.x() > 0.0) + 2*int(rel.y() > 0.0);
	}
	
	static Branch *__find_branch(Branch *root, const Key &key)
	{
		if(root->key.size >= 2*key.size && root->offspring)
		{
			return __find_branch(
			    &root->offspring[__get_index(key.position - root->key.position)],
			    key
			    );
		}
		return root;
	}
	
	static void __empty_branch(Branch *b)
	{
		bool empty = true;
		if(b->offspring)
		{
			for(int i = 0; i < 4; ++i)
			{
				empty = empty && b->offspring[i].empty;
			}
		}
		if(b->elements.size() > 0)
		{
			empty = false;
		}
		if(empty)
		{
			b->empty = true;
			if(b->parent)
			{
				__empty_branch(b->parent);
			}
		}
	}
	
	static void __fill_branch(Branch *b)
	{
		b->empty = false;
		if(b->parent)
		{
			__fill_branch(b->parent);
		}
	}
	
	static void __call_all(Branch *root, std::function<void(Branch*)> func)
	{
#ifndef __TREE_NOEMPTY
		if(!root->empty)
#endif
		{
			func(root);
			if(root->offspring)
			{
				for(int i = 0; i < 4; ++i)
				{
					__call_all(&root->offspring[i],func);
				}
			}
		}
	}
	
	static void __call_mask(Branch *root, unsigned mask, std::function<void(Branch*)> func)
	{
#ifndef __TREE_NOEMPTY
		if(!root->empty)
#endif
		{
			func(root);
			if(root->offspring)
			{
				for(int i = 0; i < 4; ++i)
				{
					if(mask & (1 << i))
					{
						__call_mask(&root->offspring[i],mask,func);
					}
				}
			}
		}
	}
	
	static void __call_next_mask(Branch *root, unsigned mask, std::function<void(Branch*)> func)
	{
#ifndef __TREE_NOEMPTY
		if(!root->empty)
#endif
		{
			func(root);
			if(root->offspring)
			{
				for(int i = 0; i < 4; ++i)
				{
					if(mask & (1 << i))
					{
						__call_all(&root->offspring[i],func);
					}
					else
					{
						unsigned next_mask = 
						    ((mask&0x1)>0)*(0x5*(i==1) | 0x3*(i==2) | 0x1*(i==3)) |
						    ((mask&0x2)>0)*(0xa*(i==0) | 0x2*(i==2) | 0x3*(i==3)) |
						    ((mask&0x4)>0)*(0xc*(i==0) | 0x4*(i==1) | 0x5*(i==3)) |
						    ((mask&0x8)>0)*(0x8*(i==0) | 0xc*(i==1) | 0xa*(i==2));
						__call_mask(&root->offspring[i],next_mask,func);
					}
				}
			}
		}
	}
	
	static void __call_neighbours(Branch *b, std::function<void(Branch*)> func)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(b->neighbours[i] 
#ifndef __TREE_NOEMPTY
			   && !b->neighbours[i]->empty
#endif
			   )
			{
				func(b->neighbours[i]);
			}
		}
	}
	
	static void __call_neighbours_index(Branch *b, std::function<void(Branch*,int)> func)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(b->neighbours[i] 
#ifndef __TREE_NOEMPTY
			   && !b->neighbours[i]->empty
#endif
			   )
			{
				func(b->neighbours[i],i);
			}
		}
	}
	
	static void __call_nearest(Branch *root, const Key &key, std::function<void(Branch*)> func)
	{
		if(root->key.isSuitable(key))
		{
			__call_all(root,func);
			__call_neighbours_index(root,[func](Branch *b, int index)
			{
				index = index + (index > 3);
				int col = index%3, row = index/3;
				unsigned mask = ~(0x5*(col==0) | 0x3*(row==0) | 0xa*(col==2) | 0xc*(row==2));
				__call_next_mask(b,mask,func);
			});
		}
		else
		if(root->key.isSuitableDown(key))
		{
#ifndef __TREE_NOEMPTY
			if(!root->empty)
#endif
			{
				func(root);
			}
			__call_neighbours(root,func);
			if(root->offspring)
			{
				__call_nearest(&root->offspring[__get_index(key.position - root->key.position)],key,func);
			}
		}
	}

#ifdef __TREE_DEBUG
	static void _draw(Branch *b)
	{
		gTranslate(fvec2(b->key.position).data);
		gTransform(fmat2(b->key.size,0,0,b->key.size).data);
		if(b->elements.size() > 0)
		{
			gSetColorInt(G_GREEN & G_ALPHA(0.1));
		}
		else
		{
			gSetColorInt(G_GREEN & G_ALPHA(0.05));
		}
		gDrawQuad();
	}
#endif
	
	static void __insert(Branch *b, const Key &key, T elem)
	{
		b->elements.push_back(std::pair<Key,T>(key,elem));
		if(b->empty)
		{
			__fill_branch(b);
		}
	}
	
	static void __remove(Branch *b, T elem)
	{
		b->elements.remove_if([elem](const std::pair<Key,T> &p)->bool{return p.second == elem;});
		__empty_branch(b);
	}
	
public:
	Tree(const Key &key, int d)
	  : depth(d)
	{
		root.key = key;
		__create_tree(&root,depth);
		__find_neighbours(&root,&root);
	}

	~Tree()
	{
		__destroy_tree(&root);
	}

	/* Iterates all elements */
	void for_all(std::function<void(std::list<std::pair<Key,T>> &)> func)
	{
		__call_all(&root,[func](Branch *b)
		{
			func(b->elements);
		});
	}
	
	/* Iterates all elements which may interact with key element */
	void for_nearest(const Key &key, std::function<void(std::list<std::pair<Key,T>> &)> func)
	{
		if(root.key.isSuitableDown(key))
		{
			__call_nearest(&root,key,[func](Branch *b)
			{
				func(b->elements);
			});
		}
	}
	
	void insert(const Key &key, T elem)
	{
		if(root.key.isSuitableDown(key))
		{
			Branch *b = __find_branch(&root,key);
			__insert(b,key,elem);
		}
	}
	
	void remove(const Key &key, T elem)
	{
		if(root.key.isSuitableDown(key))
		{
			Branch *b = __find_branch(&root,key);
			__remove(b,elem);
		}
	}
	
	void remove(T elem)
	{
		__call_all(&root,[elem](Branch *b)
		{
			__remove(b,elem);
		});
	}

	void replace(const Key &from, const Key &to, T elem)
	{
		bool move = true;
		if(root.key.isSuitableDown(from))
		{
			move = false;
			Branch *b = __find_branch(&root,from);
			if((b->offspring && !b->key.isSuitable(to)) || (!b->offspring && !b->key.isSuitableDown(to)))
			{
				move = true;
				b->elements.remove_if([elem,&move](const std::pair<Key,T> &p)->bool
				{
					return p.second == elem;
				});
				__empty_branch(b);
			}
			else
			{
				for(auto &pair : b->elements)
				{
					if(pair.second == elem)
					{
						pair.first = to;
					}
				}
			}
		}
		if(move && root.key.isSuitableDown(to))
		{
			insert(to,elem);
		}
	}
	
	/* Replaces elements if their keys were changed */
	void update()
	{
		__call_all(&root,[this](Branch *b)
		{
			b->elements.remove_if([this,b](const std::pair<Key,T> &p)
			{
				if((b->offspring && !b->key.isSuitable(p.first)) || (!b->offspring && !b->key.isSuitableDown(p.first)))
				{
					insert(p.first,p.second);
					return true;
				}
				return false;
			});
			__empty_branch(b);
		});
	}
	
#ifdef __TREE_DEBUG
	void _draw_filled()
	{
		__call_all(&root,_draw);
	}
	void _draw_nearest(const TreeKey &key)
	{
		__call_nearest(&root,key,_draw);
	}
	void _draw_neighbours(const TreeKey &key)
	{
		Branch *b = __find_branch(&root,key);
		_draw(b);
		__call_neighbours(b,_draw);
	}

#endif
};
