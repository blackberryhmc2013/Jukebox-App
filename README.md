Jukebox
===========
Jukebox is a collaborative application that allows users to create a shared playlist. It explores a variety of BlackBerry technologies such as Near Field Communication and WiFi Direct. In order to use Jukebox, the device needs BlackBerry OS 10.2.1 update and must support NFC and WiFi Direct.

Author
===========
Harvey Mudd CS BlackBerry Clinic 2013-2014 Team

cloudbase.io
===========
An account with cloudbase can be registered at https://manage.cloudbase.io/register. CloudList utilizes cloudbase.io’s native BlackBerry library available at https://github.com/cloudbase-io/CBHelper-BlackBerry10Native/. At this time, the latest cloudbase.io library is packaged with CloudList.

Registering requires an application name and password. The details necessary to use CloudList are Application Code, Unique Code (both available on the account dashboard), and the md5 hash of the Application Password chosen during creation.

Issues & Future Work
===========
* The master branch does not support WiFi Direct, as CloudList integration seemed to interfere with WiFi Direct. Visit the nonCL branch to see a version that supports WiFi Direct, but not CloudList.
* In the current version of Jukebox, the host user needs to manually load a track by selecting before playing the track. It would improve the users' experience if the application automatically loaded the tracks.
* WiFi Direct connection request is not done in the background. Each time a new WiFi Direction connection is initiated, the BlackBerry WiFi Direct API shows a dialog, which could interfere with the users' actions.
* During the design phase, the team considered allowing guest members to become the host of the playlist by NFC tapping with the host. This feature will be useful in a situation where the host leaves the rest of the users but wants the playlist to stay.
* Jukebox does not currently display album art. Extracting the album covers will involve calling lower-level APIs used in the BlackBerry's native media player or file manager to access the files' metadata.
* Jukebox does not allow users to reorder the tracks in a playlist. To implement this feature, CloudList must support inserting an object at a specific index, moving an object to another index, or swapping two objects. Each time the tracks have been reordered, the application needs to check whether the next track has changed, stop loading if the process already has started, and start loading according to the updated playlist. Users could vote for the track they wish to be played next.

Bug Reporting and Feature Requests
===========
If you find a bug, or have an enhancement request, simply file an issue in the bug tracker.

Disclaimer
===========
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
