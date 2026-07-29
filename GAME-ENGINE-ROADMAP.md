# 2D C++ Game Engine — Roadmap

**Stack:** C++ (17 or 20), CMake, SDL2 (windowing/input, already familiar)
**Goal:** A reusable, general-purpose 2D engine — not a single game's codebase
**Starting point:** You've shipped basic 2D games with raw SDL2, so the game-loop/rendering basics are assumed. This roadmap starts where "just SDL2" stops being enough and turns into engine architecture.

## How to use this

Each phase has a **goal**, **build items**, an **exit criterion** (a concrete thing you can point to and say "this phase is done"), and **key decisions** (forks in the road worth thinking about before you commit). Time estimates assume a hobby pace of a few hours a week — treat them as ratios between phases, not deadlines. Do not skip the exit criterion to move faster; an engine with untested foundations rots the fastest.

---

## Phase 0 — Foundations & Project Skeleton
*(~1–2 weeks)*

**Goal:** A project that builds cleanly on more than just your machine, before any engine code exists.

Build items:
- CMake project structure: separate `engine/` (static/shared library) from `sandbox/` or `game/` (an executable that links against the engine and is used to test it as you go)
- Dependency management: vcpkg or CPM.cmake for pulling in SDL2, GLM, etc. — avoid hand-vendoring libraries from day one
- Git repo with a `.gitignore` for build artifacts, and CI (GitHub Actions) that builds on push — catches "works on my machine" early
- Logging system (even a simple one) — you will lean on this constantly once systems start interacting
- Basic assert/error-handling convention decided now (exceptions vs. error codes vs. a `Result<T>` type) — retrofitting this later touches every file

**Exit criterion:** `cmake --build` produces a sandbox executable that opens a window and clears it to a color, on a fresh clone, on CI.

**Key decision:** Exceptions or no exceptions? Many engines disable exceptions for performance/predictability (Unreal does). If you're unsure, default to *no exceptions*, error codes/optional returns instead — easier to add exceptions later than remove them.

---

## Phase 1 — Application Layer
*(~1–2 weeks)*

**Goal:** Replace "one big SDL2 main()" with an engine-owned application shell your games plug into.

Build items:
- `Application` class owning the window, the main loop, and lifecycle (`Init` → `Run` → `Shutdown`)
- Fixed-timestep game loop with an accumulator (decouple simulation rate from render/frame rate — this matters a lot once physics shows up in Phase 5)
- Input abstraction layer: wrap SDL2 key/mouse/gamepad events behind your own `Input::IsKeyDown(Key::W)` API, not raw `SDL_SCANCODE_*` sprinkled through game code
- Event system for decoupled communication between systems (observer pattern or a simple event bus) — resize events, input events, etc. flow through this

**Exit criterion:** A sandbox game can query input and run a fixed-timestep update loop without ever calling an `SDL_*` function directly outside the engine layer.

**Key decision:** This is the point where "SDL2 wrapper" architecture gets locked in. Decide now whether the engine will ever need to swap windowing backends (e.g., GLFW, or a console SDK) — if genuinely never, it's fine to leak a *few* SDL types through the API; if you want portability, keep SDL2 entirely inside the implementation and expose only engine types.

---

## Phase 2 — Rendering Foundation
*(~3–4 weeks — this phase has the most architecturally important decision in the whole roadmap)*

**Goal:** A real 2D renderer, not `SDL_RenderCopy` calls scattered through gameplay code.

**Key decision (make this before writing code):** `SDL_Renderer` vs. raw OpenGL via SDL2's GL context.
- `SDL_Renderer`: faster to get productive, but you inherit its batching behavior and can't easily add custom shaders, post-processing, or instanced rendering later.
- Raw OpenGL (SDL2 just for the window/context): more upfront work (you write your own sprite batcher, shaders, texture atlasing), but this is what makes it a *real* renderer rather than a thin wrapper, and it's the path that scales toward lighting, particle effects, and eventually 3D if you ever want it.

For a "general-purpose engine" goal, raw OpenGL is the recommended path — it's the difference between an engine and a convenience wrapper.

Build items:
- `Renderer2D` API: `Begin()/DrawSprite()/DrawQuad()/DrawLine()/End()` — gameplay code never touches OpenGL directly
- Sprite batching (accumulate draw calls into one big vertex buffer, flush on texture change or buffer full) — this is the single biggest perf lever in a 2D engine
- Shader abstraction (compile/link/uniform-setting wrapper) even if you only have one shader today
- Camera (2D orthographic, position/zoom/rotation) and a view/projection matrix pipeline
- Texture loading (stb_image is the standard choice) and a texture atlas or array for batching

**Exit criterion:** Render 10,000+ independently-moving sprites at 60fps in the sandbox, via batched draw calls (check this with a frame counter, not a guess — this number is what tells you the batcher actually works).

**Addendum — deferring SDL_Renderer → OpenGL cheaply:** `SDL_Renderer` and raw OpenGL don't share code, so switching later always means writing a real batcher/shaders from scratch — that work doesn't shrink by waiting. What *can* be free later is everything **outside** the renderer, if you commit to one rule now: no `SDL_Texture*`, `SDL_Renderer*`, or any SDL/GL type ever leaks past a single `IRenderer2D` interface.

```cpp
class IRenderer2D {
public:
    virtual void BeginFrame(const Camera2D& cam) = 0;
    virtual void DrawSprite(TextureHandle tex, Vec2 pos, Vec2 size, float rotation) = 0;
    virtual void EndFrame() = 0;
    virtual TextureHandle LoadTexture(const std::string& path) = 0;
};

class SDLRenderer2D : public IRenderer2D { /* SDL_RenderCopy etc. — build this first */ };
class GLRenderer2D  : public IRenderer2D { /* shaders, VBOs, batching — build this later */ };
```

`TextureHandle` is your own opaque ID, never a raw pointer. As long as `Sprite` components, the asset manager's public API, and gameplay code only ever go through `IRenderer2D` + `TextureHandle`, swapping the concrete class at construction time is the entire "switch" — ECS, physics, audio, and scripting need zero changes. The one other non-renderer piece to abstract: window/context creation (`SDL_WINDOW_OPENGL` + `SDL_GL_SetAttribute` calls have to happen before window creation, `SDL_Renderer` doesn't need them), so gate that behind a small `GraphicsAPI` choice in the `Application` layer from Phase 1.

---

## Phase 3 — Resource Management
*(~1–2 weeks)*

**Goal:** Loading a texture/sound/font twice doesn't load it twice, and assets can be referenced safely without lifetime bugs.

Build items:
- `AssetManager`/`ResourceCache` keyed by path or ID, returning ref-counted or handle-based references (avoid raw pointers to assets — a handle system that can detect "this asset was unloaded" saves you from a whole category of crash)
- Async loading groundwork (even just a loading-in-a-thread queue) — retrofitting this after gameplay code assumes synchronous loads is painful
- A simple hot-reload for textures/shaders in dev builds (huge productivity win, moderate effort)

**Exit criterion:** Loading the same texture from three different game objects results in one GPU upload, verified via a log line or debug counter.

---

## Phase 4 — Entity Architecture (ECS)
*(~3–4 weeks)*

**Goal:** Move off "class Player : public GameObject" inheritance trees and onto a data-oriented entity model — this is the architectural core of a modern engine.

Build items:
- Either integrate **EnTT** (recommended — mature, header-only, extremely fast, the de facto standard in C++ hobby/indie engines) or write a minimal archetype/sparse-set ECS yourself if you want the learning experience of building one
- Core components: `Transform`, `Sprite`, `Velocity` and similar
- Systems as functions operating over component views (`RenderSystem`, `MovementSystem`) rather than methods on objects
- Entity lifecycle: creation, destruction, and *deferred* destruction (destroying entities mid-iteration is a classic bug source)

**Exit criterion:** The sandbox game's player, enemies, and projectiles are all ECS entities with no bespoke C++ classes per game-object type — behavior is composed from components + systems, not inheritance.

**Key decision:** EnTT vs. hand-rolled. If your goal is "learn how engines work," write your own sparse-set ECS once — it's one of the more approachable "build it yourself" systems and demystifies a lot. If your goal is "get to making games," use EnTT and spend the saved time on Phases 6–9 instead.

---

## Phase 5 — Physics & Collision
*(~2–3 weeks)*

**Goal:** Entities can collide and respond, without every game reimplementing AABB checks by hand.

Build items:
- Start with your own simple broad-phase (spatial hash or grid) + AABB/circle collision — don't reach for Box2D immediately, understanding this layer matters for a general-purpose engine
- Collision events fed through the Phase 1 event system (`OnCollisionEnter`, etc.), consumed by gameplay via ECS systems
- Once the simple version works end-to-end, evaluate integrating **Box2D** for real rigid-body physics (rotation, joints, restitution) rather than extending your own indefinitely

**Exit criterion:** Two moving entities collide, generate an event, and a system reacts to it (e.g., destroys one) — proving the collision → event → gameplay pipeline, not just "shapes overlap."

---

## Phase 6 — Audio
*(~1 week)*

**Goal:** Sound effects and music with no more effort than `Audio::Play("jump.wav")`.

Build items:
- Wrap SDL_mixer (fastest path) or miniaudio (more control, still simple) behind an `AudioSystem`
- Route through the Phase 3 asset manager so sounds are cached, not reloaded per play
- Basic mixing controls: per-category volume (SFX vs. music vs. master)

**Exit criterion:** Multiple overlapping sound effects play without clicking/cutting each other off, and volume sliders work per-category.

---

## Phase 7 — Scene System & Serialization
*(~2–3 weeks)*

**Goal:** Levels/scenes are data, not hardcoded C++ setup functions.

Build items:
- Scene = a saved/loadable set of entities + their component data
- Pick a serialization format (JSON via nlohmann/json is the pragmatic default; binary comes later if load times matter)
- Prefab concept: a reusable entity template (e.g., "Enemy") instantiable multiple times with overrides
- Scene transitions (loading/unloading, keeping the engine's other systems — audio, renderer — alive across the swap)

**Exit criterion:** A level can be saved to disk and reloaded, reconstructing the exact same entities/components, without any C++ recompilation.

---

## Phase 8 — Scripting Layer
*(~2–4 weeks)*

**Goal:** Gameplay logic can be iterated on without recompiling the engine — this is what separates an "engine" from "a codebase," and matters a lot for the general-purpose goal.

Build items:
- Integrate **Lua** via **sol2** (the standard, ergonomic C++/Lua binding library)
- Expose core engine APIs to Lua: entity creation/queries, input, basic transform manipulation
- Attach scripts to entities as a component (`ScriptComponent`) invoked by an ECS system each frame
- Hot-reload scripts in dev builds (pairs naturally with the Phase 3 hot-reload groundwork)

**Exit criterion:** A gameplay behavior (e.g., player movement) is entirely defined in a `.lua` file, editable and hot-reloadable without restarting the sandbox.

---

## Phase 9 — Editor & Tooling
*(~4–6 weeks — biggest phase, but this is what makes it feel like "an engine" rather than "a framework")*

**Goal:** A visual tool to build scenes, not just C++/Lua/JSON hand-editing.

Build items:
- Integrate **Dear ImGui** for all editor UI (industry standard for tool/debug UIs, minimal integration effort with an existing OpenGL renderer from Phase 2)
- Scene hierarchy panel (list/select/parent entities)
- Inspector panel (view/edit a selected entity's components)
- Viewport panel rendering the scene into an ImGui window (render-to-texture from Phase 2's renderer)
- Asset browser tied to the Phase 3 asset manager
- Save/load hooked to Phase 7's serialization

**Exit criterion:** You can build a small level entirely in the editor — placing entities, tweaking components, saving — without writing a line of C++ or Lua for level layout.

---

## Phase 10 — Polish, Profiling & Proof
*(ongoing)*

**Goal:** Prove the engine works by actually shipping something with it, and make it fast/stable enough to trust.

Build items:
- Integrate a profiler (Tracy is excellent and purpose-built for game engines) — instrument the renderer, physics, and ECS systems specifically
- Build **two or three small, deliberately different sample games** with the engine (e.g., a platformer and a top-down shooter) — this is the real test of "general-purpose"; a single game does not prove that
- Memory: replace default allocators with a simple arena/pool allocator for hot paths (entities, particles) once profiling shows it's worth it — don't do this speculatively earlier
- Documentation: even a short doc per system (what it owns, how to use it) pays for itself the moment you return to this project after a break

**Exit criterion:** Two structurally different games exist in `sandbox/` or a `samples/` folder, both built on the same engine core with no engine-side special-casing for either.

---

## Suggested library shortlist

| System | Library | Why |
|---|---|---|
| Windowing/input | SDL2 | Already know it |
| Math | GLM | Standard, header-only, matches GLSL conventions |
| ECS | EnTT | Fastest, most battle-tested C++ ECS |
| Physics | Box2D | Once you outgrow your own AABB system |
| Audio | SDL_mixer or miniaudio | Simple, proven |
| Scripting | Lua + sol2 | Most common engine scripting pairing |
| Serialization | nlohmann/json | Pragmatic default; optimize later if needed |
| Editor UI | Dear ImGui | Standard for engine tooling |
| Image loading | stb_image | Tiny, single-header, ubiquitous |
| Profiling | Tracy | Purpose-built for real-time engines |

## Sequencing notes

- Phases 0–4 are close to mandatory in this order — each depends on the previous.
- Phases 5 and 6 (physics, audio) can be reordered or done in parallel; neither depends on the other.
- Phase 8 (scripting) is more valuable *after* Phase 4 (ECS) exists, since scripts will mostly be manipulating entities/components.
- Phase 9 (editor) is deliberately late: building an editor against an engine whose core APIs are still shifting means rewriting the editor repeatedly. Resist the urge to start it early even though it's the most visually satisfying phase.
