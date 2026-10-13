# M015-U — preuves de qualification primaire

Un seul trigger FIFO a été effectué, dans qualification. Le premier préflight interrompu faute DWARF a été restauré, sans acquisition ; ses logs resume-* sont séparés des qualified-*. BIN identique après ajout DWARF, ELF différent consigné.

qualified-independent-audit.json et words/service.csv portent le décodage indépendant wire↔RAM et les marges. qualified-primary-*.bin.hex sont les bruts complets exacts ; les logs sont normalisés LF sans espaces terminaux, *.original.hex préserve tous les octets originaux. Journaux HP complets repris, hash recalculé, deux requêtes seulement. Scripts physiques conservés pour inspection, jamais à rejouer automatiquement ; hardware_primary_reference.py est leur helper original (import historique explicite dans scripts).

Les Flash2MiB, FRAM262144 et RAM intégrales restent dans l’archive locale indiquée par final-preservation.json ; leurs empreintes/intégrités sont consignées. Les buffers de capture historiques bench_result.words/xyz ne sont PAS les données de la fenêtre : les données réelles sont official_words, publiées et comparées aux payloads SPI. Aucune donnée synthétique admise.
