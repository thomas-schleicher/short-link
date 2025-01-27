document.getElementById("urlInput").addEventListener("input", function() {
    const urlInput = document.getElementById("urlInput");
    const shortenButton = document.getElementById("shortenButton");

    const urlPattern = /(https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\.[^\s]{2,}|www\.[a-zA-Z0-9][a-zA-Z0-9-]+[a-zA-Z0-9]\.[^\s]{2,}|https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9]+\.[^\s]{2,}|www\.[a-zA-Z0-9]+\.[^\s]{2,})/i

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
    const errorContainer = document.getElementById("error-container");

    if (urlInput) {
        // const shortenedUrl = urlInput + "-short"; // only for testing

        const response = await fetch("/", {
            method: "POST",
            headers: {
                "Content-Type": "text/plain",
            },
            body: urlInput,
        });
        if (response.ok){
            const shortenedUrl = await response.text();
            outputContainer.innerHTML = `<p>Shortened URL: <a href="${shortenedUrl}" target="_blank">${shortenedUrl}</a></p>`;
            errorContainer.innerHTML = "";
        }else{
            const error = await response.text();
            errorContainer.innerHTML = `<p>Something went wrong: ${error}</p>`;
            outputContainer.innerHTML = "";
        }
    }
});
