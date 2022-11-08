

// void EditorState::enter()
// {
//     Engine::getEventSystem()->register<EventKeyDown>(this, std::bind(&EditorState::onKeyDown, this));
// }

// void EditorState::leave()
// {
//     Engine::getEventSystem()->deregister<EventKeyDown>(this);
// }

// void EditorState::onKeyDown(Event* pEvent)
// {
//     auto pKeyEvent = (KeyEvent*)pEvent;
   
//     if (pKeyEvent->scanCode == SDL_SCANCODE_F1)
//         showHelp();
// }
