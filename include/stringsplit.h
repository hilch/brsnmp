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

#ifndef STRINGSPLIT_H
#define STRINGSPLIT_H
#include <string>


template <typename Container>
Container& split( Container& result, std::string &s, const typename Container::value_type& delimiters, bool empties = true )
{
  result.clear();
  size_t current;
  size_t next = -1;
  do
  {
    if (empties == false)
    {
      next = s.find_first_not_of( delimiters, next + 1 );
      if (next == Container::value_type::npos) break;
      next -= 1;
    }
    current = next + 1;
    next = s.find_first_of( delimiters, current );
    result.push_back( s.substr( current, next - current ) );
  }
  while (next != Container::value_type::npos);
  return result;
}

//usage:
//string s = "One, two,, four , five,";
//vector <string> fields;
//split( fields, s, "," );

#endif // STRINGSPLIT_H
