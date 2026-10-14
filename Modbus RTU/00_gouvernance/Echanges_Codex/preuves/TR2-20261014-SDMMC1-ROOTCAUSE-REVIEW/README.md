# Revue documentaire SDMMC1 — provenance et contrôles

Mission `TR2-20261014-P12H3H-SDMMC1-ROOTCAUSE-REVIEW` ; aucun accès matériel autorisé ni effectué.

- `source-review-provenance.json` :20fichiers tiers inspectés, SHA256, URLs épinglées ; sources non modifiées/non vendored.
- `adafruit-netlist-review.json` :extraction structurelle Eagle du schéma officiel Adafruit, commit959792478854acec21fb862650427ffaa32b07f1. Désignation R1/R2 47K (réseaux4résistances), pull-ups vers3V3 et C1 10µF/C2 100nF. Source CC-BY-SA3.0 ; attribution Adafruit, extraction informative originale publiée sous même licence pour ce fichier, pas modification du schéma.
- `qualification-PC-anterieure/` :copie documentaire des résultats acquis avant mission, provenance explicite, aucun test rejoué.
- `PROTOCOLE_LECTURE_PROPOSE.md` :unique protocole futur, **NON EXÉCUTÉ**, à autoriser dans une nouvelle mission.
- `manifest.json` :SHA256 de tous les fichiers publiés sauf lui-même, les manifests historiques inclus comme données.

ST MB1549-C05 et UM2861 Rev10 accessibles par outil web. Téléchargement urllib local du PDF ST échoué surtimeout, outils curl/pdftoppm absents ; analyse du PDF par accèsweb texte et manuel correspondant, aucun fichier source local prétendu lu. Révision physique de Nucleo/Adafruit non vérifiée. Aucun besoin de nouvelle compilation pour cette revue sans correction.
