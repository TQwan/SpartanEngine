/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ==========
#include <memory>
#include "RHI_Object.h"
//=====================

namespace Spartan
{
	class SPARTAN_CLASS RHI_ConstantBuffer : public RHI_Object
	{
	public:
		RHI_ConstantBuffer(const std::shared_ptr<RHI_Device>& rhi_device, bool is_dynamic = false)
		{
			m_rhi_device = rhi_device;
            m_is_dynamic = is_dynamic;
		}
		~RHI_ConstantBuffer();

		template<typename T>
		bool Create(const uint32_t element_count = 1)
		{
            m_stride        = static_cast<uint32_t>(sizeof(T));
            m_element_count = element_count;
            m_size_gpu      = static_cast<uint64_t>(m_stride * m_element_count);
			return _Create();
		}

		void* Map(const uint32_t offset_index = 0) const;
		bool Unmap() const;
        bool Flush(const uint32_t offset_index = 0);

		void* GetResource()         const { return m_buffer; }
        uint32_t GetStride()        const { return m_stride; }
        uint32_t GetElementCount()  const { return m_element_count; }

        // Static offset - The kind of offset that is used when updating the buffer
        uint32_t GetOffset()                                const { return m_offset_index * m_stride; }
        uint32_t GetOffsetInex()                                  { return m_offset_index; }
        void SetOffsetIndex(const uint32_t offset_index)          { m_offset_index = offset_index; }
        
        // Dynamic offset - The kind of offset that is used before a draw call, it assumes the buffer is already updated, so it only works as pointer.
        bool IsDynamic()                                        const { return m_is_dynamic; }
        uint32_t GetOffsetDynamic()                             const { return m_offset_dynamic_index * m_stride; }
        uint32_t GetOffsetIndexDynamic()                        const { return m_offset_dynamic_index; }
        void SetOffsetIndexDynamic(const uint32_t offset_index)       { m_offset_dynamic_index = offset_index; }

	private:
		bool _Create();

        bool m_is_dynamic               = false;
        uint32_t m_stride               = 0;
        uint32_t m_element_count        = 1;
        uint32_t m_offset_index         = 0;
        uint32_t m_offset_dynamic_index = 0;

		std::shared_ptr<RHI_Device> m_rhi_device;

		// API
		void* m_buffer			= nullptr;
		void* m_buffer_memory	= nullptr;
	};
}
