/*
 * Copyright (C) 2019 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _IS_SH_ROS1__INCLUDE__FACTORY_HPP_
#define _IS_SH_ROS1__INCLUDE__FACTORY_HPP_

#include <is/systemhandle/SystemHandle.hpp>

#include <ros/node_handle.h>

#include <functional>
#include <memory>

namespace eprosima {
namespace is {
namespace sh {
namespace ros1 {

/**
 * @class Factory
 *        This is a singleton class that allows to gain access to the specific
 *        publisher, subscriber, client and server conversion functions, callbacks
 *        and other tasks, for each topic and service type.
 *
 *        Coming from the ROS 1 `msg` and `srv` files, the *Integration Service*
 *        `genmsg` plugin will generate the conversion library files for each of them,
 *        after calling the `ìs_ros1_genmsg_mix` macro in the CMakeLists.txt file
 *        of the custom ROS 1 package CMake project.
 *
 *        The generated conversion files will be compiled into a dynamic library that
 *        will be registered to a `mix` file, using the MiddlewareInterfaceExtension
 *        API provided by the *Integration Service Core*.
 *        This ROS 1 conversion `mix` libraries will use this Factory class to register
 *        the conversion functions from/to ROS 1 types to xTypes, as well as the subscription,
 *        publisher, service server and service client factories, that will be used later
 *        to create the necessary links in the *core* to bridge ROS 1 with another middleware
 *        supported by the *Integration Service*.
 */
class Factory
{
public:

    /**
     * @brief Get a reference to the singleton instance of this Factory.
     *
     * @returns A mutable reference to the Factory singleton object instance.
     */
    static Factory& instance();

    /**
     * @brief Signature for the method that will be used to register a dynamic type
     *        within the types factory.
     */
    using RegisterTypeToFactory =
            std::function<xtypes::DynamicType::Ptr()>;

    /**
     * @brief Register a dynamic type within the types Factory.
     *
     * @param[in] type_name The type name, used as key in the Factory types map.
     *
     * @param[in] register_type_func The function used to create the type.
     */
    void register_type_factory(
            const std::string& type_name,
            RegisterTypeToFactory register_type_func);

    /**
     * @brief Create a dynamic type instance using the types registered previously
     *        in the Factory.
     *
     * @param[in] type_name The name of the type to bre created.
     *
     * @returns A pointer to the created type, or `nullptr` if the type was not
     *          registered in the Factory.
     */
    xtypes::DynamicType::Ptr create_type(
            const std::string& type_name);

    /**
     * @brief Signature for the method that will be used to create a ROS 1 subscription
     *        to a certain topic, within the subscriptions factory.
     *
     * @details It allows to specify the associated ROS 1 node, the topic name and type,
     *          as well as the callback function called every time a new message data
     *          arrives to this subscription.
     *
     *          This Factory method returns an opaque pointer containing the subscription
     *          object created by the *Integration Service* to manage a subscription.
     *          This subscription object is dependant on every ROS 1 type and it is
     *          autogenerated in the template available in resources/convert__msg.cpp/hpp.em.
     */
    using RegisterSubscriptionToFactory =
            std::function<std::shared_ptr<void>(
                        ros::NodeHandle& node,
                        const std::string& topic_name,
                        const xtypes::DynamicType& message_type,
                        TopicSubscriberSystem::SubscriptionCallback* callback,
                        uint32_t queue_size,
                        const ros::TransportHints& transport_hints)>;

    /**
     * @brief Register a ROS 1 subscription builder within the Factory.
     *
     * @param[in] topic_type The name of the topic type, used to index the subscription factory map.
     *
     * @param[in] register_sub_func The function used to create the subscription.
     */
    void register_subscription_factory(
            const std::string& topic_type,
            RegisterSubscriptionToFactory register_sub_func);

    /**
     * @brief Create a ROS 1 subscription handler for the *Integration Service*, using
     *        the subscriptions registered previously in the Factory.
     *
     * @param[in] topic_type A reference to the dynamic type representation of the topic type.
     *
     * @param[in] node The ROS 1 node that will hold this subscription.
     *
     * @param[in] topic_name The topic name to be subscribed to.
     *
     * @param[in] callback The callback function called every time the ROS 1 subscription
     *            receives a new data.
     *
     * @param[in] queue_size The maximum message queue size for the ROS 1 subscription.
     *
     * @param[in] transport_hints Provides the subscriber with specific transport information.
     *
     * @returns An opaque pointer to the created *Integration Service* subscription entity.
     */
    std::shared_ptr<void> create_subscription(
            const xtypes::DynamicType& topic_type,
            ros::NodeHandle& node,
            const std::string& topic_name,
            TopicSubscriberSystem::SubscriptionCallback* callback,
            uint32_t queue_size,
            const ros::TransportHints& transport_hints);

    /**
     * @brief Signature for the method that will be used to create a ROS 1 publisher
     *        to a certain topic, within the publishers factory.
     *
     * @details It allows to specify the associated ROS 1 node, the topic name to publish to,
     *          the queue size and enabling/disabling message latching.
     *
     *          This Factory method returns a pointer to an *Integration Service*
     *          TopicPublisher object, holding the created ROS 1 publisher.
     *          This publisher object is dependant on every ROS 1 type and it is
     *          autogenerated in the template available in resources/convert__msg.cpp/hpp.em.
     */
    using RegisterPublisherToFactory =
            std::function<std::shared_ptr<TopicPublisher>(
                        ros::NodeHandle& node,
                        const std::string& topic_name,
                        uint32_t queue_size,
                        bool latch)>;

    /**
     * @brief Register a ROS 1 publisher builder within the Factory.
     *
     * @param[in] topic_type The name of the topic type, used to index the publisher factory map.
     *
     * @param[in] register_pub_func The function used to create the publisher.
     */
    void register_publisher_factory(
            const std::string& topic_type,
            RegisterPublisherToFactory register_pub_func);

    /**
     * @brief Create a ROS 1 publisher handler for the *Integration Service*, using
     *        the publisher registered previously in the Factory.
     *
     * @param[in] topic_type A reference to the dynamic type representation of the topic type.
     *
     * @param[in] node The ROS 1 node that will hold this publisher.
     *
     * @param[in] topic_name The topic name to publish to.
     *
     * @param[in] queue_size The maximum message queue size for the ROS 1 publisher.
     *
     * @param[in] latch Enable/disable latching. When a connection is latched,
     *            the last message published is saved and sent to any future subscribers that connect.
     *
     * @returns A pointer to the created *Integration Service* TopicPublisher entity.
     */
    std::shared_ptr<TopicPublisher> create_publisher(
            const xtypes::DynamicType& topic_type,
            ros::NodeHandle& node,
            const std::string& topic_name,
            uint32_t queue_size,
            bool latch);

    /**
     * @brief Signature for the method that will be used to create a ROS 1 service client
     *        to a certain service, within the service clients factory.
     *
     * @details It allows to specify the associated ROS 1 node, the service name,
     *          as well as the callback function called every time a new request data
     *          arrives to this service client.
     *
     *          This Factory method returns a pointer containing the *Integration Service*
     *          ServiceClient object created by the *Integration Service* to manage a service client.
     *          This service client object is dependant on every ROS 1 type and it is
     *          autogenerated in the template available in resources/convert__srv.cpp.em.
     */
    using RegisterServiceClientToFactory =
            std::function<std::shared_ptr<ServiceClient>(
                        ros::NodeHandle& node,
                        const std::string& service_name,
                        ServiceClientSystem::RequestCallback* callback)>;

    /**
     * @brief Register a ROS 1 service client builder within the Factory.
     *
     * @param[in] service_response_type The name of the service response type,
     *            used as index in the service client factory map.
     *
     * @param[in] register_service_client_func The function used to create the service client.
     */
    void register_client_proxy_factory(
            const std::string& service_response_type,
            RegisterServiceClientToFactory register_service_client_func);

    /**
     * @brief Create a ROS 1 service client handler for the *Integration Service*, using
     *        the service client registered previously in the Factory.
     *
     * @param[in] service_response_type A reference to the dynamic type representation
     *            of the service response type.
     *
     * @param[in] node The ROS 1 node that will hold this service client.
     *
     * @param[in] service_name The service name to forward petitions to.
     *
     * @param[in] callback The callback function called every time the ROS 1 service client
     *            receives a new request.
     *
     * @returns A pointer to the created *Integration Service* ServiceClient entity.
     */
    std::shared_ptr<ServiceClient> create_client_proxy(
            const std::string& service_response_type,
            ros::NodeHandle& node,
            const std::string& service_name,
            ServiceClientSystem::RequestCallback* callback);

    /**
     * @brief Signature for the method that will be used to create a ROS 1 service server
     *        to a certain service, within the service servers factory.
     *
     * @details It allows to specify the associated ROS 1 node and the service name.
     *
     *          This Factory method returns a pointer containing the *Integration Service*
     *          ServiceProvider object created by the *Integration Service* to manage a service server.
     *          This service server object is dependant on every ROS 1 type and it is
     *          autogenerated in the template available in resources/convert__srv.cpp.em.
     */
    using RegisterServiceProviderToFactory =
            std::function<std::shared_ptr<ServiceProvider>(
                        ros::NodeHandle& node,
                        const std::string& service_name)>;

    /**
     * @brief Register a ROS 1 service server builder within the Factory.
     *
     * @param[in] service_request_type The name of the service server type to be registered.
     *
     * @param[in] register_service_server_func The function used to create the service server.
     */
    void register_server_proxy_factory(
            const std::string& service_request_type,
            RegisterServiceProviderToFactory register_service_server_func);

    /**
     * @brief Create a ROS 1 service server handler for the *Integration Service*, using
     *        the service server registered previously in the Factory.
     *
     * @param[in] service_request_type A reference to the dynamic type representation
     *            of the service request type.
     *
     * @param[in] node The ROS 1 node that will hold this service server.
     *
     * @param[in] qos_profile The QoS used to create the service server.
     *
     * @returns A pointer to the created *Integration Service* ServiceProvider entity.
     */
    std::shared_ptr<ServiceProvider> create_server_proxy(
            const std::string& service_request_type,
            ros::NodeHandle& node,
            const std::string& service_name);

private:

    /**
     * @brief Construct a new Factory object.
     *
     * @details The private constructor ensures that this class can only be
     * constructed using the `instance()` function.
     */
    Factory();

    /**
     * @class Implementation
     *        Defines the actual implementation of the Factory class.
     *
     *        Allows to use the *pimpl* procedure to separate the implementation
     *        from the interface of Factory.
     *
     *        Methods named equal to some Factory method will not be
     *        documented again. Usually, the interface class will call
     *        `_pimpl->method()`, but the functionality and parameters
     *        are exactly the same.
     */
    class Implementation;

    /**
     * Class members.
     */
    std::unique_ptr<Implementation> _pimpl;
};

/**
 * @brief FactoryRegistrar is a helper struct created to easily register a dynamic type or
 *        ROS 1 entity, namely, publisher, subscription, service server or service client.
 *
 * @tparam FactoryType The type of Factory to register a Factory method to.
 *
 * @tparam void(Factory::* register_func)(const std::string&, FactoryType) A pointer to the
 *         register method.
 */
template<typename FactoryType, void(Factory::* register_func)(const std::string&, FactoryType)>
struct FactoryRegistrar
{
    /**
     * @brief Construct a new FactoryRegistrar object.
     *
     * @param[in] type The key used to store the factory function within the corresponding Factory map.
     *
     * @param[in] factory The function to be invoked whenever a certain entity is requested to
     *            be created by the Integration Service Core (namely, a DynamicType or ROS 1 entity).
     */
    FactoryRegistrar(
            const std::string& type,
            FactoryType factory)
    {
        (Factory::instance().*register_func)(type, factory);
    }

};

//==============================================================================
using TypeToFactoryRegistrar =
        FactoryRegistrar<Factory::RegisterTypeToFactory, &Factory::register_type_factory>;

//==============================================================================
using SubscriptionToFactoryRegistrar =
        FactoryRegistrar<Factory::RegisterSubscriptionToFactory, &Factory::register_subscription_factory>;

//==============================================================================
using PublisherToFactoryRegistrar =
        FactoryRegistrar<Factory::RegisterPublisherToFactory, &Factory::register_publisher_factory>;

//==============================================================================
using ServiceClientToFactoryRegistrar =
        FactoryRegistrar<Factory::RegisterServiceClientToFactory, &Factory::register_client_proxy_factory>;

//==============================================================================
using ServiceProviderToFactoryRegistrar =
        FactoryRegistrar<Factory::RegisterServiceProviderToFactory, &Factory::register_server_proxy_factory>;


} //  namespace ros1
} //  namespace sh
} //  namespace is
} //  namespace eprosima

#endif //  _IS_SH_ROS1__INCLUDE__FACTORY_HPP_
