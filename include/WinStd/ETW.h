﻿/*
    SPDX-License-Identifier: MIT
    Copyright © 1991-2023 Amebis
    Copyright © 2016 GÉANT
*/

/// \defgroup WinStdETWAPI Event Tracing for Windows API

#pragma once

#include "Common.h"
#include <assert.h>
#include <evntprov.h>
#include <evntcons.h>
#include <stdarg.h>
#include <tdh.h>
#include <memory>
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable: 4505) // Don't warn on unused code

/// \addtogroup WinStdETWAPI
/// @{

///
/// Retrieves a property value from the event data.
///
/// \sa [TdhGetProperty function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa964843.aspx)
///
template<class _Ty, class _Ax>
static _Success_(return == ERROR_SUCCESS) ULONG TdhGetProperty(_In_ PEVENT_RECORD pEvent, _In_ ULONG TdhContextCount, _In_reads_opt_(TdhContextCount) PTDH_CONTEXT pTdhContext, _In_ ULONG PropertyDataCount, _In_reads_(PropertyDataCount) PPROPERTY_DATA_DESCRIPTOR pPropertyData, _Inout_ std::vector<_Ty, _Ax> &aData)
{
    ULONG ulSize, ulResult;

    // Query property size.
    ulResult = TdhGetPropertySize(pEvent, TdhContextCount, pTdhContext, PropertyDataCount, pPropertyData, &ulSize);
    if (ulResult == ERROR_SUCCESS) {
        if (ulSize) {
            // Query property value.
            aData.resize((ulSize + sizeof(_Ty) - 1) / sizeof(_Ty));
            ulResult = TdhGetProperty(pEvent, TdhContextCount, pTdhContext, PropertyDataCount, pPropertyData, ulSize, reinterpret_cast<PBYTE>(aData.data()));
        } else {
            // Property value size is zero.
            aData.clear();
        }
    }

    return ulResult;
}

///
/// Retrieves metadata about an event.
///
/// \sa [TdhGetEventInformation function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa964840.aspx)
///
static _Success_(return == ERROR_SUCCESS) ULONG TdhGetEventInformation(_In_ PEVENT_RECORD pEvent, _In_ ULONG TdhContextCount, _In_reads_opt_(TdhContextCount) PTDH_CONTEXT pTdhContext, _Out_ std::unique_ptr<TRACE_EVENT_INFO> &info)
{
    BYTE szBuffer[WINSTD_STACK_BUFFER_BYTES];
    ULONG ulSize = sizeof(szBuffer), ulResult;

    // Try with stack buffer first.
    ulResult = TdhGetEventInformation(pEvent, TdhContextCount, pTdhContext, (PTRACE_EVENT_INFO)szBuffer, &ulSize);
    if (ulResult == ERROR_SUCCESS) {
        // Copy from stack.
        info.reset(reinterpret_cast<PTRACE_EVENT_INFO>(new char[ulSize]));
        memcpy(info.get(), szBuffer, ulSize);
        return ERROR_SUCCESS;
    } else if (ulResult == ERROR_INSUFFICIENT_BUFFER) {
        // Create buffer on heap and retry.
        info.reset(reinterpret_cast<PTRACE_EVENT_INFO>(new char[ulSize]));
        return TdhGetEventInformation(pEvent, TdhContextCount, pTdhContext, info.get(), &ulSize);
    }

    return ulResult;
}

///
/// Retrieves information about the event map contained in the event.
///
/// \sa [TdhGetEventMapInformation function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa964841.aspx)
///
static _Success_(return == ERROR_SUCCESS) ULONG TdhGetEventMapInformation(_In_ PEVENT_RECORD pEvent, _In_ LPWSTR pMapName, _Inout_ std::unique_ptr<EVENT_MAP_INFO> &info)
{
    BYTE szBuffer[WINSTD_STACK_BUFFER_BYTES];
    ULONG ulSize = sizeof(szBuffer), ulResult;

    // Try with stack buffer first.
    ulResult = TdhGetEventMapInformation(pEvent, pMapName, (PEVENT_MAP_INFO)szBuffer, &ulSize);
    if (ulResult == ERROR_SUCCESS) {
        // Copy from stack.
        info.reset(reinterpret_cast<PEVENT_MAP_INFO>(new char[ulSize]));
        memcpy(info.get(), szBuffer, ulSize);
        return ERROR_SUCCESS;
    } else if (ulResult == ERROR_INSUFFICIENT_BUFFER) {
        // Create buffer on heap and retry.
        info.reset(reinterpret_cast<PEVENT_MAP_INFO>(new char[ulSize]));
        return TdhGetEventMapInformation(pEvent, pMapName, info.get(), &ulSize);
    }

    return ulResult;
}

/// @}

#pragma warning(pop)

namespace winstd
{
    /// \addtogroup WinStdETWAPI
    /// @{

    ///
    /// EVENT_DATA_DESCRIPTOR wrapper
    ///
    class event_data : public EVENT_DATA_DESCRIPTOR
    {
    public:
        ///
        /// Construct empty class.
        ///
        event_data()
        {
            Ptr      = 0;
            Size     = 0;
            Reserved = (ULONG)-1;   // Used for varadic argument terminator.
        }

        ///
        /// Construct class pointing to an `char`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const char &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to an `unsigned char`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const unsigned char &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to an `int`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const int &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to an `unsigned int`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const unsigned int &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to a `long`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const long &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to an `unsigned long`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const unsigned long &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to a `GUID`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_ const GUID &data)
        {
            EventDataDescCreate(this, &data, (ULONG)(sizeof(data)));
        }

        ///
        /// Construct class pointing to a string.
        ///
        /// \param[in] data  Event data. When `NULL`, the event data will be `"(null)"`.
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_opt_z_ const char *data)
        {
            if (data)
                EventDataDescCreate(this, data, (ULONG)((strlen(data) + 1) * sizeof(*data)));
            else {
                // Writing NULL pointer with 0B length causes trouble in Event Viewer: message template string is displayed only, parameters are not rendered.
                static const char null[] = "(null)";
                EventDataDescCreate(this, null, sizeof(null));
            }
        }

        ///
        /// Construct class pointing to a wide string.
        ///
        /// \param[in] data  Event data. When `NULL`, the event data will be `"(null)"`.
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_opt_z_ const wchar_t *data)
        {
            if (data)
                EventDataDescCreate(this, data, (ULONG)((wcslen(data) + 1) * sizeof(*data)));
            else {
                // Writing NULL pointer with 0B length causes trouble in Event Viewer: message template string is displayed only, parameters are not rendered.
                static const wchar_t null[] = L"(null)";
                EventDataDescCreate(this, null, sizeof(null));
            }
        }

        ///
        /// Template to construct pointing to a `std::basic_string<>`.
        ///
        /// \param[in] data  Event data
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        template<class _Elem, class _Traits, class _Ax>
        event_data(_In_ const std::basic_string<_Elem, _Traits, _Ax> &data)
        {
            EventDataDescCreate(this, data.c_str(), (ULONG)((data.length() + 1) * sizeof(_Elem)));
        }

        ///
        /// Construct class pointing to binary data.
        ///
        /// \param[in] data  Pointer to event data
        /// \param[in] size  Size of \p data in bytes
        ///
        /// \note This class saves a reference to the data only. Therefore, data must be kept available.
        ///
        #pragma warning(suppress: 26495) // EventDataDescCreate() initializes all members of EVENT_DATA_DESCRIPTOR
        event_data(_In_bytecount_(size) const void *data, _In_ ULONG size)
        {
            EventDataDescCreate(this, data, size);
        }
    };

    ///
    /// Blank event data used as terminator.
    ///
    static const event_data blank_event_data;

    ///
    /// EVENT_RECORD wrapper
    ///
    class event_rec : public EVENT_RECORD
    {
    public:
        ///
        /// Constructs a blank event record.
        ///
        event_rec()
        {
            memset((EVENT_RECORD*)this, 0, sizeof(EVENT_RECORD));
        }

        ///
        /// Copies an existing event record.
        ///
        /// \param[in] other  Event record to copy from
        ///
        event_rec(_In_ const event_rec &other) : EVENT_RECORD(other)
        {
            set_extended_data_internal(other.ExtendedDataCount, other.ExtendedData);
            set_user_data_internal(other.UserDataLength, other.UserData);
        }

        ///
        /// Copies an existing event record.
        ///
        /// \param[in] other  Event record to copy from
        ///
        event_rec(_In_ const EVENT_RECORD &other) : EVENT_RECORD(other)
        {
            set_extended_data_internal(other.ExtendedDataCount, other.ExtendedData);
            set_user_data_internal(other.UserDataLength, other.UserData);
        }

        ///
        /// Moves the event record.
        ///
        /// \param[in] other  Event record to move
        ///
        event_rec(_Inout_ event_rec&& other) noexcept : EVENT_RECORD(other)
        {
            memset((EVENT_RECORD*)&other, 0, sizeof(EVENT_RECORD));
        }

        ///
        /// Destroys event record data and frees the allocated memory.
        ///
        ~event_rec()
        {
            if (ExtendedData)
                delete reinterpret_cast<unsigned char*>(ExtendedData);

            if (UserData)
                delete reinterpret_cast<unsigned char*>(UserData);
        }

        ///
        /// Copies an existing event record.
        ///
        /// \param[in] other  Event record to copy from
        ///
        event_rec& operator=(_In_ const event_rec &other)
        {
            if (this != std::addressof(other)) {
                (EVENT_RECORD&)*this = other;
                set_extended_data_internal(other.ExtendedDataCount, other.ExtendedData);
                set_user_data_internal(other.UserDataLength, other.UserData);
            }

            return *this;
        }

        ///
        /// Copies an existing event record.
        ///
        /// \param[in] other  Event record to copy from
        ///
        event_rec& operator=(_In_ const EVENT_RECORD &other)
        {
            if (this != std::addressof(other)) {
                (EVENT_RECORD&)*this = other;
                set_extended_data_internal(other.ExtendedDataCount, other.ExtendedData);
                set_user_data_internal(other.UserDataLength, other.UserData);
            }

            return *this;
        }

        ///
        /// Moves the event record.
        ///
        /// \param[in] other  Event record to move
        ///
        event_rec& operator=(_Inout_ event_rec&& other) noexcept
        {
            if (this != std::addressof(other)) {
                (EVENT_RECORD&)*this = other;
                memset((EVENT_RECORD*)&other, 0, sizeof(EVENT_RECORD));
            }

            return *this;
        }

        ///
        /// Sets event record extended data.
        ///
        /// \param[in] count  \p data size (in number of elements)
        /// \param[in] data   Record extended data
        ///
        void set_extended_data(_In_ USHORT count, _In_count_(count) const EVENT_HEADER_EXTENDED_DATA_ITEM *data)
        {
            if (ExtendedData)
                delete reinterpret_cast<unsigned char*>(ExtendedData);

            set_extended_data_internal(count, data);
        }

        ///
        /// Sets event record user data.
        ///
        /// \param[in] size  \p data size (in bytes)
        /// \param[in] data  Record user data
        ///
        void set_user_data(_In_ USHORT size, _In_bytecount_(size) LPCVOID data)
        {
            if (UserData)
                delete reinterpret_cast<unsigned char*>(UserData);

            set_user_data_internal(size, data);
        }

    protected:
        ///
        /// Sets event record extended data.
        ///
        /// \param[in] count  \p data size (in number of elements)
        /// \param[in] data   Record extended data
        ///
        void set_extended_data_internal(_In_ USHORT count, _In_count_(count) const EVENT_HEADER_EXTENDED_DATA_ITEM *data)
        {
            if (count) {
                assert(data);

                // Count the total required memory.
                size_t data_size = 0;
                for (size_t i = 0; i < count; i++)
                    data_size += data[i].DataSize;

                // Allocate memory for extended data.
                ExtendedData = reinterpret_cast<EVENT_HEADER_EXTENDED_DATA_ITEM*>(new unsigned char[sizeof(EVENT_HEADER_EXTENDED_DATA_ITEM)*count + data_size]);

                // Bulk-copy extended data descriptors.
                memcpy(ExtendedData, data, sizeof(EVENT_HEADER_EXTENDED_DATA_ITEM) * count);

                // Copy the data.
                unsigned char *ptr = reinterpret_cast<unsigned char*>(ExtendedData + count);
                for (size_t i = 0; i < count; i++) {
                    if (data[i].DataSize) {
                        memcpy(ptr, (void*)(data[i].DataPtr), data[i].DataSize);
                        ExtendedData[i].DataPtr = (ULONGLONG)ptr;
                        ptr += data[i].DataSize;
                    } else
                        ExtendedData[i].DataPtr = NULL;
                }
            } else
                ExtendedData = NULL;

            ExtendedDataCount = count;
        }

        ///
        /// Sets event record user data.
        ///
        /// \param[in] size  \p data size (in bytes)
        /// \param[in] data  Record user data
        ///
        void set_user_data_internal(_In_ USHORT size, _In_bytecount_(size) LPCVOID data)
        {
            if (size) {
                assert(data);

                // Allocate memory for user data.
                UserData = new unsigned char[size];

                // Copy user data.
                memcpy(UserData, data, size);
            } else
                UserData = NULL;

            UserDataLength = size;
        }
    };

    ///
    /// ETW event provider
    ///
    class event_provider : public handle<REGHANDLE, NULL>
    {
        WINSTD_HANDLE_IMPL(event_provider, NULL)

    public:
        ///
        /// Closes the event provider.
        ///
        /// \sa [EventUnregister function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363749.aspx)
        ///
        virtual ~event_provider()
        {
            if (m_h != invalid)
                free_internal();
        }

        ///
        /// Registers the event provider.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when creation succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EventRegister function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363744.aspx)
        ///
        ULONG create(_In_ LPCGUID ProviderId)
        {
            handle_type h;
            ULONG ulRes = EventRegister(ProviderId, enable_callback, this, &h);
            if (ulRes == ERROR_SUCCESS)
                attach(h);
            return ulRes;
        }

        ///
        /// Writes an event with no parameters.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when write succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EventWrite function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363752.aspx)
        ///
        ULONG write(_In_ PCEVENT_DESCRIPTOR EventDescriptor)
        {
            assert(m_h != invalid);
            return EventWrite(m_h, EventDescriptor, 0, NULL);
        }

        ///
        /// Writes an event with parameters stored in array.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when write succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EventWrite function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363752.aspx)
        ///
        ULONG write(_In_ PCEVENT_DESCRIPTOR EventDescriptor, _In_ ULONG UserDataCount = 0, _In_opt_count_(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData = NULL)
        {
            assert(m_h != invalid);
            return EventWrite(m_h, EventDescriptor, UserDataCount, UserData);
        }

        ///
        /// Writes an event with one or more parameter.
        ///
        /// \note The list must be terminated with `winstd::blank_event_data`.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when write succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EventWrite function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363752.aspx)
        ///
        ULONG write(_In_ PCEVENT_DESCRIPTOR EventDescriptor, _In_ const EVENT_DATA_DESCRIPTOR param1, ...)
        {
            assert(m_h != invalid);

            // The first argument (param1) is outside of varadic argument list.
            if (param1.Ptr      == winstd::blank_event_data.Ptr      &&
                param1.Size     == winstd::blank_event_data.Size     &&
                param1.Reserved == winstd::blank_event_data.Reserved)
                return EventWrite(m_h, EventDescriptor, 0, NULL);

            va_list arg;
            va_start(arg, param1);
            va_list arg_start = arg;
            std::vector<EVENT_DATA_DESCRIPTOR> params;
            ULONG param_count;

            // Preallocate array.
            for (param_count = 1; param_count < MAX_EVENT_DATA_DESCRIPTORS; param_count++) {
                const EVENT_DATA_DESCRIPTOR &p = va_arg(arg, const EVENT_DATA_DESCRIPTOR);
                if (p.Ptr      == winstd::blank_event_data.Ptr      &&
                    p.Size     == winstd::blank_event_data.Size     &&
                    p.Reserved == winstd::blank_event_data.Reserved) break;
            }
            params.reserve(param_count);

            // Copy parameters to array.
            arg = arg_start;
            params.push_back(param1);
            for (;;) {
                const EVENT_DATA_DESCRIPTOR &p = va_arg(arg, const EVENT_DATA_DESCRIPTOR);
                if (p.Ptr      == winstd::blank_event_data.Ptr      &&
                    p.Size     == winstd::blank_event_data.Size     &&
                    p.Reserved == winstd::blank_event_data.Reserved) break;
                params.push_back(p);
            }

            va_end(arg);
#pragma warning(push)
#pragma warning(disable: 28020)
            return EventWrite(m_h, EventDescriptor, param_count, params.data());
#pragma warning(pop)
        }

        ///
        /// Writes an event with variable number of parameters.
        ///
        /// \note The list must be terminated with `winstd::blank_event_data`.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when write succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EventWrite function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363752.aspx)
        ///
        ULONG write(_In_ PCEVENT_DESCRIPTOR EventDescriptor, _In_ va_list arg)
        {
            assert(m_h != invalid);

            va_list arg_start = arg;
            std::vector<EVENT_DATA_DESCRIPTOR> params;
            ULONG param_count;

            // Preallocate array.
            for (param_count = 0; param_count < MAX_EVENT_DATA_DESCRIPTORS; param_count++) {
                const EVENT_DATA_DESCRIPTOR &p = va_arg(arg, const EVENT_DATA_DESCRIPTOR);
                if (p.Ptr      == winstd::blank_event_data.Ptr      &&
                    p.Size     == winstd::blank_event_data.Size     &&
                    p.Reserved == winstd::blank_event_data.Reserved) break;
            }
            params.reserve(param_count);

            // Copy parameters to array.
            arg = arg_start;
            for (;;) {
                const EVENT_DATA_DESCRIPTOR &p = va_arg(arg, const EVENT_DATA_DESCRIPTOR);
                if (p.Ptr      == winstd::blank_event_data.Ptr      &&
                    p.Size     == winstd::blank_event_data.Size     &&
                    p.Reserved == winstd::blank_event_data.Reserved) break;
                params.push_back(p);
            }

#pragma warning(push)
#pragma warning(disable: 28020)
            return EventWrite(m_h, EventDescriptor, param_count, params.data());
#pragma warning(pop)
        }

        ///
        /// Writes a string event.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when write succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EventWriteString function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363750v=vs.85.aspx)
        ///
        ULONG write(_In_ UCHAR Level, _In_ ULONGLONG Keyword, _In_z_ _Printf_format_string_ PCWSTR String, ...)
        {
            assert(m_h != invalid);

            std::wstring msg;
            va_list arg;

            // Format message.
            va_start(arg, String);
            vsprintf(msg, String, arg);
            va_end(arg);

            // Write string event.
            return EventWriteString(m_h, Level, Keyword, msg.c_str());
        }

    protected:
        ///
        /// Releases the event provider.
        ///
        /// \sa [EventUnregister function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363749.aspx)
        ///
        void free_internal() noexcept override
        {
            EventUnregister(m_h);
        }

        ///
        /// Receive enable or disable notification requests
        ///
        /// \sa [EnableCallback callback function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363707.aspx)
        ///
        virtual void enable_callback(_In_ LPCGUID SourceId, _In_ ULONG IsEnabled, _In_ UCHAR Level, _In_ ULONGLONG MatchAnyKeyword, _In_ ULONGLONG MatchAllKeyword, _In_opt_ PEVENT_FILTER_DESCRIPTOR FilterData)
        {
            UNREFERENCED_PARAMETER(SourceId);
            UNREFERENCED_PARAMETER(IsEnabled);
            UNREFERENCED_PARAMETER(Level);
            UNREFERENCED_PARAMETER(MatchAnyKeyword);
            UNREFERENCED_PARAMETER(MatchAllKeyword);
            UNREFERENCED_PARAMETER(FilterData);
        }

        ///
        /// Receive enable or disable notification requests
        ///
        /// \sa [EnableCallback callback function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363707.aspx)
        ///
        static VOID NTAPI enable_callback(_In_ LPCGUID SourceId, _In_ ULONG IsEnabled, _In_ UCHAR Level, _In_ ULONGLONG MatchAnyKeyword, _In_ ULONGLONG MatchAllKeyword, _In_opt_ PEVENT_FILTER_DESCRIPTOR FilterData, _Inout_opt_ PVOID CallbackContext)
        {
            if (CallbackContext)
                static_cast<event_provider*>(CallbackContext)->enable_callback(SourceId, IsEnabled, Level, MatchAnyKeyword, MatchAllKeyword, FilterData);
            else
                assert(0); // Where did the "this" pointer get lost?
        }
    };

    ///
    /// ETW session
    ///
    class event_session : public handle<TRACEHANDLE, 0>
    {
        WINSTD_NONCOPYABLE(event_session)

    public:
        ///
        /// Initializes a new empty session.
        ///
        event_session()
        {
        }

        ///
        /// Initializes a new session with an already available object handle.
        ///
        /// \param[in] h     Initial session handle value
        /// \param[in] prop  Session properties
        ///
        event_session(_In_opt_ handle_type h, _In_ const EVENT_TRACE_PROPERTIES *prop) :
            m_prop(reinterpret_cast<EVENT_TRACE_PROPERTIES*>(new char[prop->Wnode.BufferSize])),
            handle(h)
        {
            memcpy(m_prop.get(), prop, prop->Wnode.BufferSize);
        }

        ///
        /// Move constructor
        ///
        /// \param[inout] other  A rvalue reference of another session
        ///
        event_session(_Inout_ event_session &&other) noexcept :
            m_prop(std::move(other.m_prop)),
            handle(std::move(other))
        {
        }

        ///
        /// Closes the session.
        ///
        /// \sa [ControlTrace function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363696.aspx)
        ///
        virtual ~event_session()
        {
            if (m_h != invalid)
                free_internal();
        }

        ///
        /// Move assignment
        ///
        /// \param[inout] other  A rvalue reference of another object
        ///
        event_session& operator=(_Inout_ event_session &&other) noexcept
        {
            if (this != std::addressof(other)) {
                (handle<handle_type, 0>&&)*this = std::move(other);
                m_prop                          = std::move(other.m_prop);
            }
            return *this;
        }

        ///
        /// Auto-typecasting operator
        ///
        /// \return Session properties
        ///
        operator const EVENT_TRACE_PROPERTIES*() const
        {
            return m_prop.get();
        }

        ///
        /// Gets session name
        ///
        /// \return Session name
        ///
        LPCTSTR name() const
        {
            const EVENT_TRACE_PROPERTIES *prop = m_prop.get();
            return reinterpret_cast<LPCTSTR>(reinterpret_cast<LPCBYTE>(prop) + prop->LoggerNameOffset);
        }

        ///
        /// Sets a new session handle for the class
        ///
        /// When the current session handle of the class is non-NULL, the session is destroyed first.
        ///
        /// \param[in] h     New session handle
        /// \param[in] prop  Session properties
        ///
        void attach(_In_opt_ handle_type h, _In_ EVENT_TRACE_PROPERTIES *prop)
        {
            handle<handle_type, 0>::attach(h);
            m_prop.reset(prop);
        }

        ///
        /// Registers and starts an event tracing session.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when creation succeeds;
        /// - error code otherwise.
        ///
        /// \sa [StartTrace function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa364117.aspx)
        ///
        ULONG create(_In_z_ LPCTSTR SessionName, _In_ const EVENT_TRACE_PROPERTIES *Properties)
        {
            handle_type h;
            std::unique_ptr<EVENT_TRACE_PROPERTIES> prop(reinterpret_cast<EVENT_TRACE_PROPERTIES*>(new char[Properties->Wnode.BufferSize]));
            memcpy(prop.get(), Properties, Properties->Wnode.BufferSize);
            ULONG ulRes = StartTrace(&h, SessionName, prop.get());
            if (ulRes == ERROR_SUCCESS)
                attach(h, prop.release());
            return ulRes;
        }

        ///
        /// Enables the specified event trace provider.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EnableTraceEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363711.aspx)
        ///
        ULONG enable_trace(_In_ LPCGUID ProviderId, _In_ UCHAR Level, _In_opt_ ULONGLONG MatchAnyKeyword = 0, _In_opt_ ULONGLONG MatchAllKeyword = 0, _In_opt_ ULONG EnableProperty = 0, _In_opt_ PEVENT_FILTER_DESCRIPTOR EnableFilterDesc = NULL)
        {
            assert(m_h != invalid);
            return EnableTraceEx(
                ProviderId,
                &m_prop->Wnode.Guid,
                m_h,
                EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                Level,
                MatchAnyKeyword,
                MatchAllKeyword,
                EnableProperty,
                EnableFilterDesc);
        }

        ///
        /// Disables the specified event trace provider.
        ///
        /// \return
        /// - `ERROR_SUCCESS` when succeeds;
        /// - error code otherwise.
        ///
        /// \sa [EnableTraceEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363711.aspx)
        ///
        ULONG disable_trace(_In_ LPCGUID ProviderId, _In_ UCHAR Level, _In_opt_ ULONGLONG MatchAnyKeyword = 0, _In_opt_ ULONGLONG MatchAllKeyword = 0, _In_opt_ ULONG EnableProperty = 0, _In_opt_ PEVENT_FILTER_DESCRIPTOR EnableFilterDesc = NULL)
        {
            assert(m_h != invalid);
            return EnableTraceEx(
                ProviderId,
                &m_prop->Wnode.Guid,
                m_h,
                EVENT_CONTROL_CODE_DISABLE_PROVIDER,
                Level,
                MatchAnyKeyword,
                MatchAllKeyword,
                EnableProperty,
                EnableFilterDesc);
        }

    protected:
        ///
        /// Releases the session.
        ///
        /// \sa [ControlTrace function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363696.aspx)
        ///
        void free_internal() noexcept override
        {
            ControlTrace(m_h, name(), m_prop.get(), EVENT_TRACE_CONTROL_STOP);
        }

    protected:
        std::unique_ptr<EVENT_TRACE_PROPERTIES> m_prop; ///< Session properties
    };

    ///
    /// ETW trace
    ///
    /// \sa [OpenTrace function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa364089.aspx)
    ///
    class event_trace : public handle<TRACEHANDLE, INVALID_PROCESSTRACE_HANDLE>
    {
        WINSTD_HANDLE_IMPL(event_trace, INVALID_PROCESSTRACE_HANDLE)

    public:
        ///
        /// Closes the trace.
        ///
        /// \sa [CloseTrace function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363686.aspx)
        ///
        virtual ~event_trace()
        {
            if (m_h != invalid)
                free_internal();
        }

    protected:
        ///
        /// Closes the trace.
        ///
        /// \sa [CloseTrace function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363686.aspx)
        ///
        void free_internal() noexcept override
        {
            CloseTrace(m_h);
        }
    };

    ///
    /// Helper class to enable event provider in constructor and disables it in destructor
    ///
    class event_trace_enabler
    {
    public:
        ///
        /// Enables event trace
        ///
        /// \sa [EnableTraceEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363711.aspx)
        ///
        event_trace_enabler(
            _In_opt_ LPCGUID                  SourceId,
            _In_     TRACEHANDLE              TraceHandle,
            _In_     LPCGUID                  ProviderId,
            _In_     UCHAR                    Level,
            _In_opt_ ULONGLONG                MatchAnyKeyword  = 0,
            _In_opt_ ULONGLONG                MatchAllKeyword  = 0,
            _In_opt_ ULONG                    EnableProperty   = 0,
            _In_opt_ PEVENT_FILTER_DESCRIPTOR EnableFilterDesc = NULL) :
            m_provider_id(ProviderId),
            m_source_id(SourceId),
            m_trace_handle(TraceHandle),
            m_level(Level),
            m_match_any_keyword(MatchAnyKeyword),
            m_match_all_keyword(MatchAllKeyword),
            m_enable_property(EnableProperty),
            m_enable_filter_desc(EnableFilterDesc)
        {
            m_status = EnableTraceEx(
                m_provider_id,
                m_source_id,
                m_trace_handle,
                EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                m_level,
                m_match_any_keyword,
                m_match_all_keyword,
                m_enable_property,
                m_enable_filter_desc);
        }

        ///
        /// Enables event trace
        ///
        /// \sa [EnableTraceEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363711.aspx)
        ///
        event_trace_enabler(
            _In_     const event_session            &session,
            _In_           LPCGUID                  ProviderId,
            _In_           UCHAR                    Level,
            _In_opt_       ULONGLONG                MatchAnyKeyword  = 0,
            _In_opt_       ULONGLONG                MatchAllKeyword  = 0,
            _In_opt_       ULONG                    EnableProperty   = 0,
            _In_opt_       PEVENT_FILTER_DESCRIPTOR EnableFilterDesc = NULL) :
            m_provider_id(ProviderId),
            m_source_id(&((const EVENT_TRACE_PROPERTIES*)session)->Wnode.Guid),
            m_trace_handle(session),
            m_level(Level),
            m_match_any_keyword(MatchAnyKeyword),
            m_match_all_keyword(MatchAllKeyword),
            m_enable_property(EnableProperty),
            m_enable_filter_desc(EnableFilterDesc)
        {
            m_status = EnableTraceEx(
                m_provider_id,
                m_source_id,
                m_trace_handle,
                EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                m_level,
                m_match_any_keyword,
                m_match_all_keyword,
                m_enable_property,
                m_enable_filter_desc);
        }

        ///
        /// Return result of `EnableTraceEx()` call.
        ///
        /// \sa [EnableTraceEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363711.aspx)
        ///
        ULONG status() const
        {
            return m_status;
        }

        ///
        /// Disables event trace.
        ///
        /// \sa [EnableTraceEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/aa363711.aspx)
        ///
        virtual ~event_trace_enabler()
        {
            if (m_status == ERROR_SUCCESS)
                EnableTraceEx(
                    m_provider_id,
                    m_source_id,
                    m_trace_handle,
                    EVENT_CONTROL_CODE_DISABLE_PROVIDER,
                    m_level,
                    m_match_any_keyword,
                    m_match_all_keyword,
                    m_enable_property,
                    m_enable_filter_desc);
        }

    protected:
        ULONG m_status;                                 ///< Result of EnableTraceEx call
        LPCGUID m_provider_id;                          ///< Provider ID
        LPCGUID m_source_id;                            ///< Session ID
        TRACEHANDLE m_trace_handle;                     ///< Trace handle
        UCHAR m_level;                                  ///< Logging level
        ULONGLONG m_match_any_keyword;                  ///< Keyword match mask (any)
        ULONGLONG m_match_all_keyword;                  ///< Keyword match mask (all)
        ULONG m_enable_property;                        ///< Enable property
        PEVENT_FILTER_DESCRIPTOR m_enable_filter_desc;  ///< Event filter descriptor
    };

    ///
    /// Helper class to write an event on entry/exit of scope.
    ///
    /// It writes one string event at creation and another at destruction.
    ///
    class event_fn_auto
    {
    public:
        ///
        /// Writes the `event_cons` event
        ///
        event_fn_auto(_In_ event_provider &ep, _In_ const EVENT_DESCRIPTOR *event_cons, _In_ const EVENT_DESCRIPTOR *event_dest, _In_z_ LPCSTR pszFnName) :
            m_ep(ep),
            m_event_dest(event_dest)
        {
            EventDataDescCreate(&m_fn_name, pszFnName, (ULONG)(strlen(pszFnName) + 1)*sizeof(*pszFnName));
            m_ep.write(event_cons, 1, &m_fn_name);
        }

        ///
        /// Copies the object
        ///
        event_fn_auto(_In_ const event_fn_auto &other) :
            m_ep(other.m_ep),
            m_event_dest(other.m_event_dest),
            m_fn_name(other.m_fn_name)
        {
        }

        ///
        /// Moves the object
        ///
        event_fn_auto(_Inout_ event_fn_auto &&other) noexcept :
            m_ep(other.m_ep),
            m_event_dest(other.m_event_dest),
            m_fn_name(std::move(other.m_fn_name))
        {
            other.m_event_dest = NULL;
        }

        ///
        /// Writes the `event_dest` event
        ///
        ~event_fn_auto()
        {
            if (m_event_dest)
                m_ep.write(m_event_dest, 1, &m_fn_name);
        }

        ///
        /// Copies the object
        ///
        event_fn_auto& operator=(_In_ const event_fn_auto &other)
        {
            if (this != &other) {
                assert(&m_ep == &other.m_ep);
                m_event_dest = other.m_event_dest;
                m_fn_name    = other.m_fn_name;
            }

            return *this;
        }

        ///
        /// Moves the object
        ///
        event_fn_auto& operator=(_Inout_ event_fn_auto &&other) noexcept
        {
            if (this != &other) {
                assert(&m_ep == &other.m_ep);
                m_event_dest = other.m_event_dest;
                m_fn_name    = std::move(other.m_fn_name);
                other.m_event_dest = NULL;
            }

            return *this;
        }

    protected:
        event_provider &m_ep;                   ///< Reference to event provider in use
        const EVENT_DESCRIPTOR *m_event_dest;   ///< Event descriptor at destruction
        EVENT_DATA_DESCRIPTOR m_fn_name;        ///< Function name
    };

    ///
    /// Helper template to write an event on entry/exit of scope with one parameter (typically result).
    ///
    /// It writes one string event at creation and another at destruction, with allowing one sprintf type parameter for string event at destruction.
    ///
    template<class T>
    class event_fn_auto_ret
    {
    public:
        ///
        /// Writes the `event_cons` event
        ///
        event_fn_auto_ret(_In_ event_provider &ep, _In_ const EVENT_DESCRIPTOR *event_cons, _In_ const EVENT_DESCRIPTOR *event_dest, _In_z_ LPCSTR pszFnName, T &result) :
            m_ep(ep),
            m_event_dest(event_dest),
            m_result(result)
        {
            EventDataDescCreate(m_desc + 0, pszFnName, (ULONG)(strlen(pszFnName) + 1)*sizeof(*pszFnName));
            m_ep.write(event_cons, 1, m_desc);
        }

        ///
        /// Copies the object
        ///
        event_fn_auto_ret(_In_ const event_fn_auto_ret<T> &other) :
            m_ep(other.m_ep),
            m_event_dest(other.m_event_dest),
            m_result(other.m_result)
        {
            m_desc[0] = other.m_desc[0];
        }

        ///
        /// Moves the object
        ///
        event_fn_auto_ret(_Inout_ event_fn_auto_ret<T> &&other) :
            m_ep(other.m_ep),
            m_event_dest(other.m_event_dest),
            m_result(other.m_result)
        {
            m_desc[0] = std::move(other.m_desc[0]);
            other.m_event_dest = NULL;
        }

        ///
        /// Writes the `event_dest` event
        ///
        ~event_fn_auto_ret()
        {
            if (m_event_dest) {
                EventDataDescCreate(m_desc + 1, &m_result, sizeof(T));
                m_ep.write(m_event_dest, 2, m_desc);
            }
        }

        ///
        /// Copies the object
        ///
        event_fn_auto_ret& operator=(_In_ const event_fn_auto_ret<T> &other)
        {
            if (this != &other) {
                assert(&m_ep == &other.m_ep);
                m_event_dest = other.m_event_dest;
                m_desc[0] = other.m_desc[0];
                assert(&m_result == &other.m_result);
            }

            return *this;
        }

        ///
        /// Moves the object
        ///
        event_fn_auto_ret& operator=(_Inout_ event_fn_auto_ret<T> &&other)
        {
            if (this != &other) {
                assert(&m_ep == &other.m_ep);
                m_event_dest = other.m_event_dest;
                m_desc[0] = std::move(other.m_desc[0]);
                assert(&m_result == &other.m_result);
                other.m_event_dest = NULL;
            }

            return *this;
        }

    protected:
        event_provider &m_ep;                   ///< Reference to event provider in use
        const EVENT_DESCRIPTOR *m_event_dest;   ///< Event descriptor at destruction
        EVENT_DATA_DESCRIPTOR m_desc[2];        ///< Function name and return value
        T &m_result;                            ///< Function result
    };

    /// @}
}
