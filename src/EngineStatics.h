#pragma once

class World;
class InputSystem;
class D3DRenderer;
class Logger;
class FontManager;
class EventSystem;
class PhysicsSystem;
class TextureManager;

class EngineStatics
{
	friend class D3DApp;

public:
	static D3DRenderer* getRenderer();
	static World* getWorld();
	static InputSystem* getInputSystem();
};