<%namespace name="shared" file="shared_components.mako" />
<%
    from django.core.urlresolvers import reverse
%>

${shared.header('Upload datasets and JARs', section='upload')}

<div id="upload_index" class="view">
    <div class="cos-upload_choices">
        <p>
            Upload your data and MapReduce tasks to the platform.
        </p>
        <ul>
            <li><a href="/filebrowser/upload?dest=${ datasets_base }&next=${ url('cosmos.views.upload_index') }"
                   data-filters="ArtButton" target="_blank"
                   data-icon-styles="{'width': 16, 'height': 16, 'top':4,
                                      'left': 5}"
                   class="cos-upload_dataset">Upload dataset</a>
                or browse <a href="/filebrowser/view/${ datasets_base }"
                             target="FileBrowser">existing datasets</a>.
            </li>
            <li><a href="/filebrowser/upload?dest=${ jars_base }&next=${ url('cosmos.views.upload_index') }"
                   data-filters="ArtButton" target="_blank"
                   data-icon-styles="{'width': 16, 'height': 16, 'top':4,
                                      'left': 5}"
                   class="cos-upload_jar">Upload jar</a>
                or browse <a href="/filebrowser/view/${ jars_base }"
                             target="FileBrowser">existing JARs</a>.
            </li>
        </ul>
    </div>
</div>

<div class="cos-uploader jframe-hidden">
    <a class="fb-cancel-upload">Close</a>
    <ul class="cos-upload-list"></ul>

    <div class="cos-noflash">If you are experiencing flash errors due to uploading,
        you can <a target="hue_upload" href="${ url('filebrowser.views.upload') }">upload without flash</a>.
    </div>
</div>

${shared.footer()}
## vim:set syntax=mako et:
