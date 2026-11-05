# Complément V1.9 — photos utilisateur des séparations de connecteurs C05

Date : 2026-11-05. Réponse à la question « quelle paire présente 4 mm ? » sous forme de **deux photos des deux flancs de la Nucleo MB1549 C05, avec règle**.

## Pièces communiquées dans la conversation

- `IMG_1255.jpeg` : vue rapprochée du flanc comportant les blocs **CN7/CN10** (rangées visibles 10+17), contexte du groupe portant les sérigraphies **USART / SDMMC** ; règle posée à proximité.
- `IMG_1256.jpeg` : vue rapprochée du flanc comportant les blocs **CN8/CN9** (rangées visibles 8+17), sérigraphies **CAN / I2C / A0 / VIN**, règle posée à proximité.

Les photos sont dans la conversation ChatGPT de l'utilisateur, **pas dans ce dépôt Git ni nécessairement accessibles à Codex localement**. Elles donnent une identification physique plus utile des deux paires ; **elles ne sont pas un relevé métrologique certifié** pour 1 mm de différence à cause de perspective, de déformation de l'optique et du fait que la règle n'est pas un axe de référence exactement coplanaire. **Ne pas inventer une cote de 4 mm ou 5 mm assignée définitivement à une paire à partir des seuls pixels.**

## Observation utilisateur associée

Le gabarit à barre de contrôle mesurée **exactement 100 mm** a les distances de référence J2→J3 et J1→J4 toutes deux **5,08 mm entre centres de broches extrêmes adjacentes** (d'après `docs/comparaison-C05.csv`). L'utilisateur observe les espacements physiques **environ 4 mm pour une paire, environ 5 mm pour l'autre**, ce qui semble expliquer un décalage de 1 mm dans le papier. Les distances évoquées par l'utilisateur peuvent désigner un espace libre entre corps plastiques : **ne pas confondre espace physique et intervalle centre-à-centre**. Photos utiles pour montrer où regarder, **pas suffisantes pour trancher formellement les valeurs métriques**.

**Action Codex :** inspecter les fichiers officiels fabricant ou les photos comme contre-vérification seulement, déterminer les entraxes officiels C05, puis attribuer précisément les écarts aux blocs et aux axes. Si les téléchargements restent inaccessibles, ne pas annoncer une correction fictive, et ne pas demander à l'utilisateur de nouvelles photographies sans justification. Maintenir NO-GO fabricant ; pas de déplacement de connecteurs sur une estimation photographique. Le contrat actif V1.9 et les autres archives restent inchangés.
