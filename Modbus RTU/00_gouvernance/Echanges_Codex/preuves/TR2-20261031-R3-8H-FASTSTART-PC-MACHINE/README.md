# FASTSTART 8H — checkpoint de préparation

[PRET_8H.md](PRET_8H.md) donne la commande locale et les limites. Les helpers sont les scripts exacts utilisés/préparés sur T1000, archivés pour traçabilité ; **ne pas les exécuter depuis ce dossier de preuves**, leurs chemins locaux sont intentionnellement spécifiques. La commande opératoire est celle de la note PRET. Le long est interdit avant le signal utilisateur LANCE8H.

`arm/r3_coex.bin` est l'image réellement flashée ; l'ELF et la MAP restent dans le dossier local tracé, leurs empreintes sont dans artifact.json. Aucune dépendance algorithmique changée. `build.log` : build ciblé, avertissements nosys _read/_write du linker nano (pas d'échec), succès effectif. `validation-reuse.json` : contrôles déjà acquis, non répétés.

Les traces gate/dumps bornés sont distincts des anciennes campagnes : nouveaux nomsN8A seulement, aucune longue capture ici. Le fichiergate est entièrement relu viaFatFs/SHA ; aucune extractionPC ni qualification8h annoncée. Les quelques préfixesSPI/FIR sont uniquement ceux du nouveau gate.

`remote-prepared.json` sera ajouté après relecture du premier checkpoint publié. Le rapport reste IN_PROGRESS jusqu'au signal puis à la récupération/qualification ; prêt au départ ne signifie pas DONE8h.

Sorties outils Windows/Linux originales conservées **octet pour octet en zlib**, suffixe `.log.zlib` ou `.txt.zlib` : CRLF/encodage/espaces ne sont pas normalisés. RAW_LOGS.json donne leurs empreintes après décompression (`zlib.decompress`). Les copies locales opérationnelles restent non compressées. Aucun échec de validation masqué : cette compression préserve les preuves brutes.
