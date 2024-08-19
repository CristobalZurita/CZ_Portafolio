document.addEventListener("DOMContentLoaded", function() {
  console.log("DOM fully loaded");
  
  let scrollContainer = document.querySelector(".gallery");
  let backBtn = document.getElementById("backBtn");
  let nextBtn = document.getElementById("nextBtn");

  console.log("scrollContainer:", scrollContainer);
  console.log("backBtn:", backBtn);
  console.log("nextBtn:", nextBtn);

  if(scrollContainer && backBtn && nextBtn) {
    console.log("All elements found");

    nextBtn.addEventListener('click', () => {
      console.log("Next button clicked");
      scrollContainer.scrollLeft += 300;
    });

    backBtn.addEventListener('click', () => {
      console.log("Back button clicked");
      scrollContainer.scrollLeft -= 300;
    });
  } else {
    console.log("Some elements are missing");
  }
});