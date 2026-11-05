# Rectificatif ChatGPT — vérification réelle des ressources ST MB1549

Date : 2026-11-05, durant la mission `TR2-20261105-SHIELD-V19-C05-OFFICIAL-CAD`.

## Correction d'une affirmation trop forte

ChatGPT avait présenté « les fichiers de conception ST existent » et avait décrit leurs versions comme une solution disponible **sans avoir réussi à ouvrir et à inspecter les ZIP**. L'utilisateur a signalé à juste titre que ce n'est **pas** une preuve d'accessibilité. La mission V1.9 n'est pas modifiée pendant l'exécution ; ce rectificatif distingue les vérifications.

1. **Page produit accessible et consultée** : https://www.st.com/en/evaluation-tools/nucleo-u575zi-q.html . Elle **annonce** `MB1549 Board design project files v4.0` du 25/01/2024 et `MB1549 Manufacturing files v3.4` du 24/02/2025. Cela démontre **un référencement catalogue**, pas un téléchargement exploitable.
2. **PDF C05 effectivement consultable** : https://www.st.com/resource/en/schematic_pack/mb1549-u575ziq-c05-schematic.pdf ; la première page montre `MB1549`, `U575ZIQ`, `C 05`, datée du 12/07/2023. C'est un schéma électrique, **pas** un relevé métrique certifié des connecteurs.
3. Les liens ZIP exacts obtenus en suivant ceux de la page ST sont :
   - https://www.st.com/resource/en/board_manufacturing_specification/mb1549-bdp.zip
   - https://www.st.com/resource/en/board_manufacturing_specification/mb1549-manufacturing.zip
   Le moteur de consultation web a refusé d'afficher le contenu avec **`400 Unsupported content-type: application/zip`** ; ce code est une **limite de l'outil**, pas la preuve d'une erreur HTTP 400 renvoyée par ST. Un autre essai de téléchargement HTTPS direct depuis le conteneur a échoué par erreur de connexion. Aucun ZIP n'a été ouvert, aucune empreinte calculée, aucune version/revision C05 interne vérifiée.
4. Codex signale sur son poste T1000 des expirations de connexion aux téléchargements directs ST et une erreur HTTP du navigateur Edge ; il étudie d'autres chemins ST. **Ces constats sont distincts de ceux de ChatGPT** et l'issue n'est pas encore connue.

## Conséquences de rigueur

**Disponibilité documentaire annoncée : OUI. Disponibilité du schéma PDF C05 : OUI. Disponibilité de la CAO/production ZIP téléchargeable et exploitable : NON VERIFIÉE. Géométrie C05 extraite/confirmée : NON.**

Codex ne doit pas déplacer J1–J5 sans preuve métrique C05. Si les ZIP sont inaccessibles, rechercher une source officielle alternative ou isoler l'impossibilité dans le rapport et demander **une seule donnée pertinente**, sans présenter un lien catalogue comme source téléchargée. Maintenir NO-GO fabrication. Ce rectificatif ne modifie ni l'ancien rapport Codex, ni le contrat de mission actif, ni le PCB.
