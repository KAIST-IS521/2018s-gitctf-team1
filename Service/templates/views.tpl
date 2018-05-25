<table class="table">
<thead><tr><th>IDX</th><th>HASH</th></tr></thead><tbody>
%s
</tbody>
</table>
<div class="modal" tabindex="-1" role="dialog" id="modall">
 <div class="modal-dialog" role="document" style="max-width: 600px; text-align: left">
  <div class="modal-content">
     <div class="modal-header">
       <button type="button" class="close" data-dismiss="modal" aria-label="Close">
         <span aria-hidden="true">&times;</span>
       </button>
     </div>
     <div class="modal-body">
       <xmp></xmp>
     </div>
     <div class="modal-footer">
       <button type="button" class="btn btn-secondary" data-dismiss="modal">Close</button>
     </div>
   </div>
 </div>
</div>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js"></script>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js"></script>
<script>function open_modal(idx) { $.post("view.py", "idx=" + idx, function(d){$("xmp").text(d) }); $('#modall').modal('show') }</script>
