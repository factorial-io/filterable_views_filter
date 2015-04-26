/**
 * @file
 * Handles Views' exposed form AJAX data submission.
 */


(function($) {

  /**
   * Loads content from url and attaches behaviours properly.
   * @see filterable_views_filter_ajax_callback
   */
  $.fn.pROJECTLoad = function(url, selector) {
    jQuery(selector).load(url, function() {
      var content = jQuery(selector);
      Drupal.attachBehaviors(content);
    });
  };

  Drupal.behaviors.filterable_views_filter = {
    attach: function() {
      $('.state-selector').change(function(e){
        var formSelector = '.' + this.value;
        $(formSelector).removeClass('element-hidden').addClass('element-not-hidden');
      });
      $('.state-close').click(function(e){
        var filter = $(e.target).parents('[class^="filter-"]');
        var classes = filter.attr("class").split(' ');

        $.each(Drupal.settings.filterable_views_filter, function(index, value){
          if ($.inArray(index, classes) != -1){
            $(filter).find(':input').val(value);
          }
        });
        var id = $(this).parents('.element-not-hidden').attr('id');
        $(this).parents('.element-not-hidden').addClass('element-hidden')
      });
    }
  };

})(jQuery);
