document.addEventListener("DOMContentLoaded", function() {
  console.log("DOM fully loaded");

  let gallery = document.querySelector(".gallery");
  let previewImage = document.querySelector(".preview img");
  let thumbnails = document.querySelectorAll(".gallery img");
  let backBtn = document.getElementById("backBtn");
  let nextBtn = document.getElementById("nextBtn");
  let currentIndex = 0;

// Función para actualizar la imagen principal y la miniatura activa
function updatePreview(index) {
  if (index >= 0 && index < thumbnails.length) {
    previewImage.src = thumbnails[index].src;
    currentIndex = index;

    // Remover la clase 'active' de todas las miniaturas
    thumbnails.forEach(thumb => thumb.classList.remove('active'));
    // Agregar la clase 'active' a la miniatura seleccionada
    thumbnails[index].classList.add('active');
  }
}

  // Evento para actualizar la imagen cuando se hace clic en una miniatura
  thumbnails.forEach((thumbnail, index) => {
    thumbnail.addEventListener('click', () => {
      updatePreview(index);
    });
  });

  // Eventos para los botones "Back" y "Next"
  nextBtn.addEventListener('click', () => {
    let nextIndex = (currentIndex + 1) % thumbnails.length;
    updatePreview(nextIndex);
  });

  backBtn.addEventListener('click', () => {
    let prevIndex = (currentIndex - 1 + thumbnails.length) % thumbnails.length;
    updatePreview(prevIndex);
  });

  // Inicializar con la primera imagen
  updatePreview(currentIndex);
});
