document.getElementById("urlInput").addEventListener("input", function() {
    const urlInput = document.getElementById("urlInput");
    const shortenButton = document.getElementById("shortenButton");

    const urlPattern = /^(https?:\/\/)?([\w\d-]+\.)+[a-z]{2,6}(\/[\w\d-]*)*\/?$/i;
    const isValid = urlPattern.test(urlInput.value);

    if (isValid) {
        urlInput.style.borderColor = "green";
        shortenButton.disabled = false;
    } else {
        urlInput.style.borderColor = "red";
        shortenButton.disabled = true;
    }
});

document.getElementById("shortenButton").addEventListener("click", async function() {
    const urlInput = document.getElementById("urlInput").value;
    const outputContainer = document.getElementById("output-container");

    if (urlInput) {
        // const shortenedUrl = urlInput + "-short"; // only for testing

        const shortenedUrl = await fetch("0.0.0.0:8080/", {
            method: "POST",
            headers: {
                "Content-Type": "text/plain",
            },
            body: urlInput,
        });

        outputContainer.innerHTML = `<p>Shortened URL: <a href="${shortenedUrl}" target="_blank">${shortenedUrl}</a></p>`;
    }
});
