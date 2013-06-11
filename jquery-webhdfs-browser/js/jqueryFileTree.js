// jQuery File Tree Plugin
//
// Version 1.01
//
// Cory S.N. LaViska
// A Beautiful Site (http://abeautifulsite.net/)
// 24 March 2008
//
// Visit http://abeautifulsite.net/notebook.php?article=58 for more information
//
// Usage: $('.fileTreeDemo').fileTree( options, callback )
//
// Options:  root           - root folder to display; default = /
//           script         - location of the serverside AJAX file to use; default = jqueryFileTree.php
//           folderEvent    - event to trigger expand/collapse; default = click
//           expandSpeed    - default = 500 (ms); use -1 for no animation
//           collapseSpeed  - default = 500 (ms); use -1 for no animation
//           expandEasing   - easing function to use on expand (optional)
//           collapseEasing - easing function to use on collapse (optional)
//           multiFolder    - whether or not to limit the browser to one subfolder at a time
//           loadMessage    - Message to display while initial tree loads (can be HTML)
//
// History:
//
// 1.01 - updated to work with foreign characters in directory/file names (12 April 2008)
// 1.00 - released (24 March 2008)
//
// TERMS OF USE
// 
// This plugin is dual-licensed under the GNU General Public License and the MIT License and
// is copyright 2008 A Beautiful Site, LLC. 
//
;define(['jquery'], function($) {
    'use strict';
    $.extend($.fn, {
        fileTree: function(o) {
            // Defaults
            if( !o ) var o = {};
            if( !o.root ) o.root = '/';
            if( !o.handler ) {
                o.handler = {
                    listDir: function(path) {
                        var result = $.Deferred();
                        result.resolve([]);
                        return result;
                    },
                    getFile: function(path) {
                        return;
                    }
                }
            }
            if( !o.folderEvent ) o.folderEvent = 'click';
            if( !o.expandSpeed ) o.expandSpeed= 500;
            if( !o.collapseSpeed ) o.collapseSpeed= 500;
            if( !o.expandEasing ) o.expandEasing = null;
            if( !o.collapseEasing ) o.collapseEasing = null;
            if( !o.multiFolder ) o.multiFolder = true;
            if( !o.loadMessage ) o.loadMessage = 'Loading...';
            
            $(this).each( function() {

                function getClass(item) {
                    switch (item.type) {
                        case 'FILE':
                        case 'file':
                            return 'file ext_' + item.pathSuffix.split('.').pop();
                        case 'DIRECTORY':
                        case 'directory':
                            return 'directory collapsed';
                    }
                }
                
                function showTree(c, t) {
                    $(c).addClass('wait');
                    $(".jqueryFileTree.start").remove();
                    o.handler.listDir(t).then(function(data) {
                        var htmlData = data.map(function(item) {
                            var itemClass = getClass(item);
                            return '<li class="' + itemClass + '">' + 
                                '   <a href="#" rel="' + item.pathSuffix + '">'+ item.pathSuffix + '</a>' +
                                '</li>';
                        });
                        $(c).find('.start').html('');
                        var ul = $('<ul class="jqueryFileTree" style="display: none;">').append(htmlData);
                        $(c).removeClass('wait').append(ul);
                        if( o.root == t ) $(c).find('UL:hidden').show(); else $(c).find('UL:hidden').slideDown({ duration: o.expandSpeed, easing: o.expandEasing });
                        bindTree(c);
                    });
                }
                
                function bindTree(t) {
                    $(t).find('LI A').bind(o.folderEvent, function() {
                        if( $(this).parent().hasClass('directory') ) {
                            if( $(this).parent().hasClass('collapsed') ) {
                                // Expand
                                if( !o.multiFolder ) {
                                    $(this).parent().parent().find('UL').slideUp({ duration: o.collapseSpeed, easing: o.collapseEasing });
                                    $(this).parent().parent().find('LI.directory').removeClass('expanded').addClass('collapsed');
                                }
                                $(this).parent().find('UL').remove(); // cleanup
                                showTree( $(this).parent(), escape($(this).attr('rel').match( /.*\// )) );
                                $(this).parent().removeClass('collapsed').addClass('expanded');
                            } else {
                                // Collapse
                                $(this).parent().find('UL').slideUp({ duration: o.collapseSpeed, easing: o.collapseEasing });
                                $(this).parent().removeClass('expanded').addClass('collapsed');
                            }
                        } else {
                            o.handler.getFile($(this).attr('rel'));
                        }
                        return false;
                    });
                    // Prevent A from triggering the # on non-click events
                    if( o.folderEvent.toLowerCase != 'click' ) $(t).find('LI A').bind('click', function() { return false; });
                }
                // Loading message
                $(this).html('<ul class="jqueryFileTree start"><li class="wait">' + o.loadMessage + '<li></ul>');
                // Get the initial file list
                showTree( $(this), escape(o.root) );
            });
        }
    });
    
});