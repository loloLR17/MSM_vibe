# Extraction PC réelle R3-8H — qualité rejetée

Les archives zlib et `BUNDLE.json` conservent les données réelles et leurs SHA-256 bruts/stockés. `EXTENTS.json` donne offsets, tailles, SHA originaux PC et tous les fragments du header, des records et du footer. La queue préallouée inutilisée n'est pas publiée (anciens octets potentiellement privés) ; aucune reconstruction à queue nulle n'est l'original PC.

Le long est **REJECTED_QUALITY**. L'intégrité de ses octets n'autorise ni COMPLETE ni qualification de mesure. Le lecteur strict et les critères restent inchangés. Le court est COMPLETE.

Pour une revue indépendante, décompresser les fragments après comparaison des deux SHA du BUNDLE ; créer exclusivement un nouveau fichier de `original_file_bytes` octets à queue nulle et écrire chaque `parts` à son `offset`. Lire le court avec `experiments/r3_durable_fat32/tools/read_campaign.py` ; le long doit lever `rail or clipping in completed capture`. Pour contrôler aussi tous les champs v3 et le SHA externe après ce rejet, utiliser `inspect_quality_failure.py` avec les RAM result.json et source-sha.bin de `terminal/BUNDLE.json` (sources e8-long-data/result.json et e8-long-data/source-sha.bin). Aucun accès SD/MCU requis.

Les métadonnées PC/FORENSIC.json localisent record980435, son contexte binaire/CSV et l'unique candidat aux limites int16. Les copies originales entières restent hors dépôt ; leur SHA inclut les octets inutilisés et ne peut être recalculé depuis une reconstruction publique.
