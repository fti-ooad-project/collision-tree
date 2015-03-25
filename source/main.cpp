#include <list>

#include <4u/la/mat.hpp>
#include <4u/random/contrand.hpp>
#include <media/media.h>
#include <graphics/graphics.h>

#define __TREE_DEBUG
//#define __TREE_NOEMPTY
#include "tree.hpp"

struct State
{
	int done = 0;
	int wait = 0;
	int width = 0;
	int height = 0;
	bool ready = false;
	
	Tree<int> *tree = nullptr;
	TreeKey key;
};

void handleAppEvent(const Media_AppEvent *event, void *data)
{
	State *state = static_cast<State*>(data);
	switch(event->type)
	{
	case MEDIA_APP_SAVESTATE:
		printInfo("Save state\n");
		break;
	case MEDIA_APP_QUIT:
		state->done = 1;
		printInfo("Quit\n");
		break;
	case MEDIA_APP_SHOW:
		state->wait = 0;
		printInfo("Show\n");
		break;
	case MEDIA_APP_HIDE:
		state->wait = 1;
		printInfo("Hide\n");
		break;
	default:
		break;
	}
}

void handleSurfaceEvent(const Media_SurfaceEvent *event, void *data)
{
	State *state = static_cast<State*>(data);
	switch(event->type)
	{
	case MEDIA_SURFACE_INIT:
		printInfo("Init surface\n");
		gInit();
		gSetBlendMode(G_BLEND_ADDITION);
		break;
	case MEDIA_SURFACE_TERM:
		printInfo("Term surface\n");
		gDispose();
		break;
	case MEDIA_SURFACE_RESIZE:
		printInfo("Resize surface ( %d, %d )\n",event->w,event->h);
		state->width = event->w;
		state->height = event->h;
		gResize(event->w,event->h);
		state->ready = true;
		break;
	default:
		break;
	}
}

void handleMotionEvent(const Media_MotionEvent *event, void *data)
{
	State *state = static_cast<State*>(data);
	TreeKey new_key;
	switch(event->action)
	{
	case MEDIA_ACTION_UP:
		//printInfo("Up\n");
		break;
	case MEDIA_ACTION_DOWN:
		// printInfo("Down(index: %d, button: %d, pos: (%d,%d))\n",event->index,event->button,event->x,event->y);
		break;
	case MEDIA_ACTION_MOVE:
		// printInfo("Move(index: %d, pos: (%d,%d))\n",event->index,event->x,event->y);
		new_key = TreeKey(vec2(event->x,event->y),state->key.size);
		state->tree->replace(state->key,new_key,0);
		state->key = new_key;
		break;
	case MEDIA_ACTION_WHEEL:
		new_key = state->key;
		if(event->yval > 0)
		{
			new_key.size *= 1.2;
		}
		else
		if(event->yval < 0)
		{
			new_key.size /= 1.2;
		}
		state->tree->replace(state->key,new_key,0);
		state->key = new_key;
		break;
	default:
		break;
	}
	//printInfo("Motion ( %d, %d )\n", static_cast<int>(pos.x()), static_cast<int>(pos.y()));
}

void handleSensorEvent(const Media_SensorEvent *event, void *data)
{
	switch(event->sensor)
	{
	case MEDIA_SENSOR_ACCELEROMETER:
		// printInfo("Accelerometer ( %f, %f, %f)\n",event->x,event->y,event->z);
		break;
	default:
		break;
	}
}

void render(Media_App *app)
{
	State *state = static_cast<State*>(app->data);
	gClear();
#ifdef __TREE_DEBUG
	state->tree->_draw_nearest(state->key);
#endif
	gSetColorInt(G_BLUE & G_ALPHA(0.6));
	state->tree->for_all([](std::list<std::pair<TreeKey,int>> &list)
	{
		for(auto pair : list)
		{
			gTranslate(fvec2(pair.first.position).data);
			gTransform(fmat2(unifmat2*pair.first.size).data);
			gDrawCircle();
		}
	});
	gSetColorInt(G_RED & G_ALPHA(1.0));
	state->tree->for_nearest(state->key,[](std::list<std::pair<TreeKey,int>> &list)
	{
		for(auto pair : list)
		{
			gTranslate(fvec2(pair.first.position).data);
			gTransform(fmat2(unifmat2*pair.first.size).data);
			gDrawCircle();
		}
	});
}

int Media_main(Media_App *app)
{
	State state;
	double size = 400.0;
	Tree<int> tree = Tree<int>(TreeKey(nullvec2,size),7);
	ContRand rand;
	
	state.tree = &tree;
	
	state.key = TreeKey(nullvec2,16);
	tree.insert(state.key,0);
	for(int i = 0; i < 0x200; ++i)
	{
		double rad = 3.0*rand.get();
		rad = rad*rad + 2.0;
		tree.insert(TreeKey(2.0*size*(vec2(rand.get(),rand.get()) - vec2(0.5,0.5)),rad),i+1);
	}
	
	app->data = static_cast<void*>(&state);
	
	Media_Listener listener;
	listener.app = &handleAppEvent;
	listener.surface = &handleSurfaceEvent;
	listener.motion = &handleMotionEvent;
	listener.sensor = &handleSensorEvent;
	listener.data = &state;
	Media_addListener(app,&listener);

	app->renderer = &render;
	
	for(;;)
	{
		if(state.wait)
		{
			Media_waitForEvent(app);
		}
		Media_handleEvents(app);

		if(state.done)
		{
			break;
		}

		if(state.ready)
		{
			
		}

		// printInfo("Frame\n");
		Media_renderFrame(app);
	}
	
	return 0;
}
