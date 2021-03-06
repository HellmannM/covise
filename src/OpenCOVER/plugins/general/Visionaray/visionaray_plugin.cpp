/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include <cassert>

#include <iostream>
#include <memory>
#include <ostream>

#include <boost/algorithm/string.hpp>

#include <osg/Sequence>

#include <config/CoviseConfig.h>

#include <cover/coVRAnimationManager.h>
#include <cover/coVRPluginSupport.h>
#include <cover/VRViewer.h>

#include <OpenVRUI/coCheckboxGroup.h>
#include <OpenVRUI/coCheckboxMenuItem.h>
#include <OpenVRUI/coRowMenu.h>
#include <OpenVRUI/coSliderMenuItem.h>
#include <OpenVRUI/coSubMenuItem.h>

#include "drawable.h"
#include "state.h"
#include "visionaray_plugin.h"

namespace visionaray
{

    //-------------------------------------------------------------------------------------------------
    // Private implementation
    //

    struct Visionaray::impl : vrui::coMenuListener
    {

        using check_box = std::unique_ptr<vrui::coCheckboxMenuItem>;
        using menu = std::unique_ptr<vrui::coMenu>;
        using radio_button = std::unique_ptr<vrui::coCheckboxMenuItem>;
        using radio_group = std::unique_ptr<vrui::coCheckboxGroup>;
        using slider = std::unique_ptr<vrui::coSliderMenuItem>;
        using sub_menu = std::unique_ptr<vrui::coSubMenuItem>;

        impl()
            : drawable_ptr(new drawable)
        {
            init_state_from_config();
            drawable_ptr->update_state(state, dev_state);
        }

        osg::Node::NodeMask objroot_node_mask;
        osg::ref_ptr<osg::Geode> geode;
        osg::ref_ptr<drawable> drawable_ptr;

        struct
        {
            menu main_menu;
            menu algo_menu;
            menu device_menu;
            menu dev_menu;
            sub_menu main_menu_entry;
            sub_menu algo_menu_entry;
            sub_menu device_menu_entry;
            sub_menu dev_menu_entry;

            // main menu
            check_box toggle_update_mode;
            check_box toggle_color_space;
            slider bounces_slider;

            // algo menu
            radio_group algo_group;
            radio_button simple_button;
            radio_button whitted_button;
            radio_button pathtracing_button;

            // device menu
            radio_group device_group;
            radio_button cpu_button;
            radio_button gpu_button;

            // dev menu
            check_box suppress_rendering;
            check_box toggle_bvh_display;
            radio_group debug_kernel_group;
            check_box toggle_bvh_costs_display;
            check_box toggle_geometric_normal_display;
            check_box toggle_shading_normal_display;
            check_box toggle_tex_coord_display;
        } ui;

        std::shared_ptr<render_state> state;
        std::shared_ptr<debug_state> dev_state;

        // init

        void init_state_from_config();
        void init_ui();

        // menu listener interface

        void menuEvent(vrui::coMenuItem *item);

        // control state

        void set_data_variance(data_variance var);
        void set_color_space(color_space cs);
        void set_algorithm(algorithm algo);
        void set_num_bounces(unsigned num_bounces);
        void set_device(device_type dev);
        void set_suppress_rendering(bool suppress_rendering);
        void set_show_bvh(bool show_bvh);
        void set_show_bvh_costs(bool show_costs);
        void set_show_geometric_normals(bool show_geometric_normals);
        void set_show_shading_normals(bool show_shading_normals);
        void set_show_tex_coords(bool show_tex_coords);
    };

    //-------------------------------------------------------------------------------------------------
    // Read state from COVISE config
    //

    void Visionaray::impl::init_state_from_config()
    {

        //
        //
        // <Visionaray>
        //     <DataVariance value="static"  />                 <!-- "static" | "dynamic" -->
        //     <Algorithm    value="simple"  />                 <!-- "simple" | "whitted" -->
        //     <Framebuffer  colorSpace="sRGB" />               <!-- colorSpace: "sRGB" | "RGB" -->
        //     <NumBounces   value="4" min="1" max="10" />      <!-- value:Integer | [min:Integer|max:Integer]  -->
        //     <Device       value="CPU"     />                 <!-- "CPU"    | "GPU"     -->
        //     <CPUScheduler numThreads="16" />                 <!-- numThreads:Integer   -->
        // </Visionaray>
        //
        //

        state = std::make_shared<render_state>();
        dev_state = std::make_shared<debug_state>();

        // Read config

        using boost::algorithm::to_lower;

        auto algo_str = covise::coCoviseConfig::getEntry("COVER.Plugin.Visionaray.Algorithm");
        auto num_bounces = covise::coCoviseConfig::getInt("value", "COVER.Plugin.Visionaray.NumBounces", 4);
        auto min_bounces = covise::coCoviseConfig::getInt("min", "COVER.Plugin.Visionaray.NumBounces", 1);
        auto max_bounces = covise::coCoviseConfig::getInt("max", "COVER.Plugin.Visionaray.NumBounces", 10);
        auto device_str = covise::coCoviseConfig::getEntry("COVER.Plugin.Visionaray.Device");
        auto data_var_str = covise::coCoviseConfig::getEntry("COVER.Plugin.Visionaray.DataVariance");
        auto clr_space_str = covise::coCoviseConfig::getEntry("colorSpace", "COVER.Plugin.Visionaray.Framebuffer");
        auto num_threads = covise::coCoviseConfig::getInt("numThreads", "COVER.Plugin.Visionaray.CPUScheduler", 0);

        to_lower(algo_str);
        to_lower(device_str);
        to_lower(data_var_str);
        to_lower(clr_space_str);

        // Update state

        if (algo_str == "whitted")
        {
            state->algo = Whitted;
        }
        else if (algo_str == "pathtracing")
        {
            state->algo = Pathtracing;
        }
        else
        {
            state->algo = Simple;
        }

        // TODO
        //  assert( min_bounces <= num_bounces && num_bounces <= max_bounces );

        state->num_bounces = num_bounces;
        state->min_bounces = min_bounces;
        state->max_bounces = max_bounces;

        if (device_str == "gpu")
        {
            state->device = GPU;
        }
        else
        {
            state->device = CPU;
        }

        state->data_var = data_var_str == "dynamic" ? Dynamic : AnimationFrames;
        state->num_threads = num_threads;

        if (clr_space_str == "rgb")
        {
            state->clr_space = RGB;
        }
        else
        {
            state->clr_space = sRGB;
        }
    }

    void Visionaray::impl::init_ui()
    {
        using namespace vrui;

        ui.main_menu_entry.reset(new coSubMenuItem("Visionaray..."));
        opencover::cover->getMenu()->add(ui.main_menu_entry.get());

        // main menu

        ui.main_menu.reset(new coRowMenu("Visionaray", opencover::cover->getMenu()));
        ui.main_menu_entry->setMenu(ui.main_menu.get());

        ui.toggle_update_mode.reset(new coCheckboxMenuItem("Update scene per frame", state->data_var == Dynamic));
        ui.toggle_update_mode->setMenuListener(this);
        ui.main_menu->add(ui.toggle_update_mode.get());

        ui.toggle_color_space.reset(new coCheckboxMenuItem("Output sRGB", state->clr_space == sRGB));
        ui.toggle_color_space->setMenuListener(this);
        ui.main_menu->add(ui.toggle_color_space.get());

        ui.bounces_slider.reset(new coSliderMenuItem("Number of bounces", state->min_bounces, state->max_bounces, state->num_bounces));
        ui.bounces_slider->setInteger(true);
        ui.bounces_slider->setMenuListener(this);
        ui.main_menu->add(ui.bounces_slider.get());

        // algorithm submenu

        ui.algo_menu_entry.reset(new coSubMenuItem("Rendering algorithm..."));
        ui.main_menu->add(ui.algo_menu_entry.get());

        ui.algo_menu.reset(new coRowMenu("Rendering algorithm", ui.main_menu.get()));
        ui.algo_menu_entry->setMenu(ui.algo_menu.get());

        ui.algo_group.reset(new coCheckboxGroup(/* allow empty selection: */ false));

        ui.simple_button.reset(new coCheckboxMenuItem("Simple", state->algo == Simple, ui.algo_group.get()));
        ui.simple_button->setMenuListener(this);
        ui.algo_menu->add(ui.simple_button.get());

        ui.whitted_button.reset(new coCheckboxMenuItem("Whitted", state->algo == Whitted, ui.algo_group.get()));
        ui.whitted_button->setMenuListener(this);
        ui.algo_menu->add(ui.whitted_button.get());

        ui.pathtracing_button.reset(new coCheckboxMenuItem("Pathtracing", state->algo == Pathtracing, ui.algo_group.get()));
        ui.pathtracing_button->setMenuListener(this);
        ui.algo_menu->add(ui.pathtracing_button.get());

        // device submenu

        ui.device_menu_entry.reset(new coSubMenuItem("Device..."));
        ui.main_menu->add(ui.device_menu_entry.get());

        ui.device_menu.reset(new coRowMenu("Device", ui.main_menu.get()));
        ui.device_menu_entry->setMenu(ui.device_menu.get());

        ui.device_group.reset(new coCheckboxGroup(/* allow empty selection: */ false));

        ui.cpu_button.reset(new coCheckboxMenuItem("CPU", state->device == CPU, ui.device_group.get()));
        ui.cpu_button->setMenuListener(this);
        ui.device_menu->add(ui.cpu_button.get());

        ui.gpu_button.reset(new coCheckboxMenuItem("GPU", state->device == GPU, ui.device_group.get()));
        ui.gpu_button->setMenuListener(this);
        ui.device_menu->add(ui.gpu_button.get());

        // dev submenu at the bottom!

        if (dev_state->debug_mode)
        {
            ui.dev_menu_entry.reset(new coSubMenuItem("Developer..."));
            ui.main_menu->add(ui.dev_menu_entry.get());

            ui.dev_menu.reset(new coRowMenu("Developer", ui.main_menu.get()));
            ui.dev_menu_entry->setMenu(ui.dev_menu.get());

            ui.suppress_rendering.reset(new coCheckboxMenuItem("Suppress rendering with Visionaray", false));
            ui.suppress_rendering->setMenuListener(this);
            ui.dev_menu->add(ui.suppress_rendering.get());

            ui.toggle_bvh_display.reset(new coCheckboxMenuItem("Show BVH outlines", false));
            ui.toggle_bvh_display->setMenuListener(this);
            ui.dev_menu->add(ui.toggle_bvh_display.get());

            ui.debug_kernel_group.reset(new coCheckboxGroup(/* allow empty selection: */ true));

            ui.toggle_bvh_costs_display.reset(new coCheckboxMenuItem("Show BVH traversal costs", false, ui.debug_kernel_group.get()));
            ui.toggle_bvh_costs_display->setMenuListener(this);
            ui.dev_menu->add(ui.toggle_bvh_costs_display.get());

            ui.toggle_geometric_normal_display.reset(new coCheckboxMenuItem("Show geometric normals", false, ui.debug_kernel_group.get()));
            ui.toggle_geometric_normal_display->setMenuListener(this);
            ui.dev_menu->add(ui.toggle_geometric_normal_display.get());

            ui.toggle_shading_normal_display.reset(new coCheckboxMenuItem("Show shading normals", false, ui.debug_kernel_group.get()));
            ui.toggle_shading_normal_display->setMenuListener(this);
            ui.dev_menu->add(ui.toggle_shading_normal_display.get());

            ui.toggle_tex_coord_display.reset(new coCheckboxMenuItem("Show texture coordinates", false, ui.debug_kernel_group.get()));
            ui.toggle_tex_coord_display->setMenuListener(this);
            ui.dev_menu->add(ui.toggle_tex_coord_display.get());
        }
    }

    void Visionaray::impl::menuEvent(vrui::coMenuItem *item)
    {
        // main menu
        if (item == ui.toggle_update_mode.get())
        {
            set_data_variance(ui.toggle_update_mode->getState() ? Dynamic : Static);
        }

        if (item == ui.toggle_color_space.get())
        {
            set_color_space(ui.toggle_color_space->getState() ? sRGB : RGB);
        }

        // algorithm submenu
        if (item == ui.simple_button.get())
        {
            set_algorithm(Simple);
        }
        else if (item == ui.whitted_button.get())
        {
            set_algorithm(Whitted);
        }
        else if (item == ui.pathtracing_button.get())
        {
            set_algorithm(Pathtracing);
        }

        if (item == ui.bounces_slider.get())
        {
            set_num_bounces(ui.bounces_slider->getValue());
        }

        // device submenu
        if (item == ui.cpu_button.get())
        {
            set_device(CPU);
        }
        else if (item == ui.gpu_button.get())
        {
            set_device(GPU);
        }

        // dev submenu
        if (item == ui.suppress_rendering.get())
        {
            set_suppress_rendering(ui.suppress_rendering->getState());
        }

        if (item == ui.toggle_bvh_display.get())
        {
            set_show_bvh(ui.toggle_bvh_display->getState());
        }

        if (item == ui.toggle_bvh_costs_display.get())
        {
            set_show_bvh_costs(ui.toggle_bvh_costs_display->getState());
        }
        else if (item == ui.toggle_geometric_normal_display.get())
        {
            set_show_geometric_normals(ui.toggle_geometric_normal_display->getState());
        }
        else if (item == ui.toggle_shading_normal_display.get())
        {
            set_show_shading_normals(ui.toggle_shading_normal_display->getState());
        }
        else if (item == ui.toggle_tex_coord_display.get())
        {
            set_show_tex_coords(ui.toggle_tex_coord_display->getState());
        }
    }

    //-------------------------------------------------------------------------------------------------
    // Control state
    //

    void Visionaray::impl::set_data_variance(data_variance var)
    {
        state->data_var = var;
        ui.toggle_update_mode->setState(var == Dynamic, false);
    }

    void Visionaray::impl::set_color_space(color_space cs)
    {
        state->clr_space = cs;
        ui.toggle_color_space->setState(cs == sRGB, false);
    }

    void Visionaray::impl::set_algorithm(algorithm algo)
    {
        state->algo = algo;
        ui.simple_button->setState(algo == Simple, false);
        ui.whitted_button->setState(algo == Whitted, false);
        ui.pathtracing_button->setState(algo == Pathtracing, false);
    }

    void Visionaray::impl::set_num_bounces(unsigned num_bounces)
    {
        state->num_bounces = num_bounces;
        ui.bounces_slider->setValue(num_bounces);
    }

    void Visionaray::impl::set_device(device_type dev)
    {
        state->device = dev;
        ui.cpu_button->setState(dev == CPU, false);
        ui.gpu_button->setState(dev == GPU, false);
    }

    void Visionaray::impl::set_suppress_rendering(bool suppress_rendering)
    {
        drawable_ptr->set_suppress_rendering(suppress_rendering);
        ui.suppress_rendering->setState(suppress_rendering, false);
    }

    void Visionaray::impl::set_show_bvh(bool show_bvh)
    {
        dev_state->show_bvh = show_bvh;
        ui.toggle_bvh_display->setState(show_bvh, false);
    }

    void Visionaray::impl::set_show_bvh_costs(bool show_costs)
    {
        dev_state->show_bvh_costs = show_costs;
        ui.toggle_bvh_costs_display->setState(show_costs, false);
    }

    void Visionaray::impl::set_show_geometric_normals(bool show_geometric_normals)
    {
        dev_state->show_geometric_normals = show_geometric_normals;
        ui.toggle_geometric_normal_display->setState(show_geometric_normals, false);
    }

    void Visionaray::impl::set_show_shading_normals(bool show_shading_normals)
    {
        dev_state->show_shading_normals = show_shading_normals;
        ui.toggle_shading_normal_display->setState(show_shading_normals, false);
    }

    void Visionaray::impl::set_show_tex_coords(bool show_tex_coords)
    {
        dev_state->show_tex_coords = show_tex_coords;
        ui.toggle_tex_coord_display->setState(show_tex_coords, false);
    }

    //-------------------------------------------------------------------------------------------------
    // Visionaray plugin
    //

    Visionaray::Visionaray()
        : impl_(new impl)
    {
    }

    Visionaray::~Visionaray()
    {
        opencover::cover->getObjectsRoot()->setNodeMask(impl_->objroot_node_mask);
        if (impl_->geode)
        {
            impl_->geode->removeDrawable(impl_->drawable_ptr);
            opencover::cover->getScene()->removeChild(impl_->geode);
        }
    }

    bool Visionaray::init()
    {
        using namespace osg;

        opencover::VRViewer::instance()->culling(false);

        std::cout << "Init Visionaray Plugin!!" << std::endl;

        impl_->init_ui();

        impl_->geode = new osg::Geode;
        impl_->geode->setName("Visionaray");
        impl_->geode->addDrawable(impl_->drawable_ptr);

        impl_->objroot_node_mask = opencover::cover->getObjectsRoot()->getNodeMask();

        opencover::cover->getScene()->addChild(impl_->geode);

        return true;
    }

    void Visionaray::addNode(osg::Node *node, opencover::RenderObject *obj)
    {
        impl_->state->rebuild = true;
    }

    void Visionaray::removeNode(osg::Node *node, bool isGroup, osg::Node *realNode)
    {
        impl_->state->rebuild = true;
    }

    void Visionaray::preDraw(osg::RenderInfo &info)
    {
        if (impl_->state->data_var == Dynamic)
            impl_->state->rebuild = true;

        if (impl_->state->rebuild)
        {
            auto seqs = opencover::coVRAnimationManager::instance()->getSequences();
            impl_->drawable_ptr->acquire_scene_data(seqs);
            impl_->state->rebuild = false;
        }

        impl_->state->animation_frame = opencover::coVRAnimationManager::instance()->getAnimationFrame();
    }

    void Visionaray::expandBoundingSphere(osg::BoundingSphere &bs)
    {
        impl_->drawable_ptr->expandBoundingSphere(bs);
    }

    void Visionaray::key(int type, int key_sym, int /* mod */)
    {
        if (type == osgGA::GUIEventAdapter::KEYDOWN)
        {
            switch (key_sym)
            {
            case '1':
                impl_->set_algorithm(Simple);
                break;

            case '2':
                impl_->set_algorithm(Whitted);
                break;

            case '3':
                impl_->set_algorithm(Pathtracing);
                break;
            }
        }
    }

} // namespace visionaray

COVERPLUGIN(visionaray::Visionaray)
