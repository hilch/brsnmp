/*
 * brsnmp - PVI diagnosis tool for B&R PLCs
 *
 * Repository: https://github.com/hilch/brsnmp
 *
 * SPDX-License-Identifier: GPL-3.0-only
 * Licensed under the GNU General Public License version 3.
 * See LICENSE for details.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef PLC_H
#define PLC_H

#include <string>

struct Plc
{
    std::string m_targetTypeDescription;
    std::string m_macAddress;
    std::string m_ipAddress;
    std::string m_subnetMask;
    std::string m_arVersion;
    bool m_INA_activated;
    int  m_INA_nodeNumber;
    int m_INA_portNumber;
};

#endif // PLC_H
