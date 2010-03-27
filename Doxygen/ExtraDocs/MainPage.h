/**
 * \mainpage Epiar developer API
 * \section s_intro Introduction
 * Welcome to the Epiar development API. This page will introduce to the various subsystems of Epiar and familiarize you with Epiar's inner workings. As always, we welcome code contribution to the Epiar project, but we ask that you use existing components and try to follow the style of the code.
 *
 * Epiar is written in both C++ and Lua.  We use C++ to code the base functionality and the performance critical sections, and Lua to implement a high level scripting interface that allows extensibility of Epiar.
 *
 * \section s_codestruct Code Structure
 * Epiar is divided into two major categories of code, the game engine code - which are utility components for encapsulating common operations to the game, and the game code - which deals within the Epiar universe.  The line between the two types of code isn't always clear cut, as some components may do a little of the other.
 *
 * \subsection ss_baseengine Game Engine
 * The base engine subsystems are shown below, these are parts of the code that interacts directly with the operating system. Click on one to see the documentation for its class.
 * \dot
 * digraph engine{
 *	node [shape=record, fontname=Helvetica, fontsize=10];
 *	Video[URL="\ref Video"];
 *	Audio[URL="\ref Audio"];
 *	File[URL="\ref File"];
 *	Lua[URL="\ref Lua"];
 *	Input[URL="\ref Input"];
 *
 *	Engine -> Video;
 *	Engine -> Audio;
 *	Engine -> File;
 *	Engine -> Lua;
 *	Engine -> Input;
 * }
 * \enddot
 *
 * In addition to the base subsystem classes, there are some classes that can also be considered part of the engine rather than the game.
 * - \subpage uipage "User Interface" - Creates common user interface widgets.
 * - \ref Xml class - XML parsing and saving for configuration data.
 * - \ref Font class - Draws texts to the screen.
 * - \ref Animation class - Creates various effects.
 * - \ref Simulation class - The game loop.
 * - \ref Sprite class - Handles sprites.
 *
 * \subsection ss_game Game Components
 *
 * \section Contact Contact Information
 * http://www.epiar.net
 */

// vim:tw=0:wrap
