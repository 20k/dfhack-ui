// This is a generic plugin that does nothing useful apart from acting as an example... of a plugin that does nothing :D

// some headers required for a plugin. Nothing special, just the basics.
#include "Core.h"
#include <Console.h>
#include <Export.h>
#include <PluginManager.h>

#include <map>

// DF data structure definition headers
#include "DataDefs.h"

#include "modules/Items.h"
#include "modules/Maps.h"
#include "modules/Units.h"
#include "modules/World.h"

#include "df/manager_order.h"
#include "df/creature_raw.h"
#include "df/world.h"

using namespace DFHack;
using namespace DFHack::Items;
using namespace DFHack::Units;
using namespace df::enums;

struct ClothingRequirement
{
    df::job_type job_type;
    df::item_type item_type;
    int16_t item_subtype;
    int16_t needed_per_citizen;
    std::map<int32_t, int32_t> total_needed_per_size;
};

std::vector<ClothingRequirement>clothingOrders;

// A plugin must be able to return its name and version.
// The name string provided must correspond to the filename -
// skeleton.plug.so, skeleton.plug.dylib, or skeleton.plug.dll in this case
DFHACK_PLUGIN("autoclothing");

// Any globals a plugin requires (e.g. world) should be listed here.
// For example, this line expands to "using df::global::world" and prevents the
// plugin from being loaded if df::global::world is null (i.e. missing from symbols.xml):
//
REQUIRE_GLOBAL(world);

// Only run if this is enabled
DFHACK_PLUGIN_IS_ENABLED(autoclothing_enabled);

// Here go all the command declarations...
// mostly to allow having the mandatory stuff on top of the file and commands on the bottom
command_result autoclothing(color_ostream &out, std::vector <std::string> & parameters);

static void do_autoclothing();

// Mandatory init function. If you have some global state, create it here.
DFhackCExport command_result plugin_init(color_ostream &out, std::vector <PluginCommand> &commands)
{
    // Fill the command list with your commands.
    commands.push_back(PluginCommand(
        "autoclothing", "Automatically manage clothing work orders",
        autoclothing, false, /* true means that the command can't be used from non-interactive user interface */
        // Extended help string. Used by CR_WRONG_USAGE and the help command:
        "  This command does nothing at all.\n"
        "Example:\n"
        "  skeleton\n"
        "    Does nothing.\n"
    ));
    return CR_OK;
}

// This is called right before the plugin library is removed from memory.
DFhackCExport command_result plugin_shutdown(color_ostream &out)
{
    // You *MUST* kill all threads you created before this returns.
    // If everything fails, just return CR_FAILURE. Your plugin will be
    // in a zombie state, but things won't crash.
    return CR_OK;
}

// Called to notify the plugin about important state changes.
// Invoked with DF suspended, and always before the matching plugin_onupdate.
// More event codes may be added in the future.

DFhackCExport command_result plugin_onstatechange(color_ostream &out, state_change_event event)
{
    switch (event) {
    case SC_WORLD_LOADED:
        // initialize from the world just loaded
        break;
    case SC_WORLD_UNLOADED:
        // cleanup
        break;
    default:
        break;
    }
    return CR_OK;
}


// Whatever you put here will be done in each game step. Don't abuse it.
// It's optional, so you can just comment it out like this if you don't need it.

DFhackCExport command_result plugin_onupdate ( color_ostream &out )
{
    if (!autoclothing_enabled)
        return CR_OK;

    if (!Maps::IsValid())
        return CR_OK;

    if (DFHack::World::ReadPauseState())
        return CR_OK;

    if ((world->frame_counter + 500) % 1200 != 0) // Check every day, but not the same day as other things
        return CR_OK;

    do_autoclothing();

    return CR_OK;
}


// A command! It sits around and looks pretty. And it's nice and friendly.
command_result autoclothing(color_ostream &out, std::vector <std::string> & parameters)
{
    // It's nice to print a help message you get invalid options
    // from the user instead of just acting strange.
    // This can be achieved by adding the extended help string to the
    // PluginCommand registration as show above, and then returning
    // CR_WRONG_USAGE from the function. The same string will also
    // be used by 'help your-command'.
    if (!parameters.empty())
        return CR_WRONG_USAGE;
    // Commands are called from threads other than the DF one.
    // Suspend this thread until DF has time for us. If you
    // use CoreSuspender, it'll automatically resume DF when
    // execution leaves the current scope.
    CoreSuspender suspend;
    // Actually do something here. Yay.
    out.print("Hello! I do nothing, remember?\n");
    // Give control back to DF.
    return CR_OK;
}

static void do_autoclothing()
{
    if (clothingOrders.size() == 0)
        return;

    for (auto&& unit : world->units.active)
    {
        if (!isCitizen(unit))
            continue;
        for (auto&& clothingOrder : clothingOrders)
        {
            int alreadyOwnedAmount = 0;

            for (auto&& ownedItem : unit->owned_items)
            {
                auto item = findItemByID(ownedItem);

                if (item->getType() != clothingOrder.item_type)
                    continue;
                if (item->getSubtype() != clothingOrder.item_subtype)
                    continue;

                alreadyOwnedAmount++;
            }

            alreadyOwnedAmount -= clothingOrder.needed_per_citizen;

            if (alreadyOwnedAmount <= 0)
                continue;
        }
    }
}
