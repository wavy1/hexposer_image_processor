(function(){
    document.querySelector('.mg-button-progress').addEventListener('click', function( event ) {
      var mgButtonProgress = document.querySelector('.mg-button-progress');
      if (mgButtonProgress) {
        mgButtonProgress.classList.toggle('running');
      }
    }, false);
  }());