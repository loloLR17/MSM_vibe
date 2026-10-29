# R1-ARM micro — observations physiques acquises sur TR2-002

Image source0c4b8699, BIN9ffe22…507e6 ; ELF et vraies capturesDWT fournis. Un flash normal et un déclenchementRAM ; aucune répétition du benchmark. Capture initiale échouée sur le nom de fichierGDB quoted aprèsDONE ; correction de collecte uniquement, capture suivante auDONE existant. Les deux journaux sont conservés, pas de nouveau RUN/reset/flash pour cette erreur de collecte.

Les`.zlib` sont les octets exacts des pièces ; BUNDLE donne SHA256 brut/stocké. La sauvegarde Flash initiale2MiB reste locale, hash/vecteurs/trace fournis ; préfixe21860byte-identique au probeSDMMC-B historiquefdb7fa…df829. La nouvelle image a été vérifiée par programmateur puis readback exact auWAIT.

HCLK configuré160MHz, timingIRQ/instrumentation inclus ; pas une calibrationSI. Distributions brutes conservées, statistiques cibles recomputées indépendamment et appariements validés. Le bloc128sources dépasse1ms enmicro ; pas de qualificationFIFO/FRAM/RTU/SD anticipée. Le testhôte utilise une faussehorloge, explicitement distinct des captures physiques. PremièreAPIprimaire constatéeperdue avant ledebug/flashsecond ; actionphysiquerestaurationdemandée, pas de manipulation primaire parCodex.
